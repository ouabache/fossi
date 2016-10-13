/* Copyright (C) 2015 Cotton Seed
   
   This file is part of arachne-pnr.  Arachne-pnr is free software;
   you can redistribute it and/or modify it under the terms of the GNU
   General Public License version 2 as published by the Free Software
   Foundation.
   
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

#include "util.hh"
#include "chipdb.hh"
#include "netlist.hh"
#include "pcf.hh"
#include "line_parser.hh"
#include "designstate.hh"
#include "casting.hh"

#include <cstring>
#include <istream>
#include <fstream>

class PCFParser : public LineParser
{
  const Package &package;
  Model *top;
  Constraints &constraints;
  
public:
  PCFParser(const std::string &f, std::istream &s_, DesignState &ds)
    : LineParser(f, s_),
      package(ds.package),
      top(ds.top),
      constraints(ds.constraints)
  {}
  
  void parse();
};

void
PCFParser::parse()
{
  std::map<std::string, Location> net_pin_loc;
  std::map<Location, std::string> pin_loc_net;
  
  for (;;)
    {
      if (eof())
        break;
      
      read_line();
      if (words.empty())
        continue;
      
      const std::string &cmd = words[0];
      if (cmd == "set_io")
        {
          bool err_no_port = true;
          
          const char *net_name = nullptr,
            *pin_name = nullptr;
          for (int i = 1; i < (int)words.size(); ++i)
            {     
              if (words[i][0] == '-')
                {
                  if (words[1] == "--warn-no-port")
                    err_no_port = false;
                  else
                    fatal(fmt("unknown option `" << words[1] << "'"));
                }
              else
                {
                  if (net_name == nullptr)
                    net_name = words[i].c_str();
                  else if (pin_name == nullptr)
                    pin_name = words[i].c_str();
                  else
                    fatal("set_io: too many arguments");
                }
            }
          
          if (!net_name || !pin_name)
            fatal("set_io: too few arguments");
          
          Port *p = top->find_port(net_name);
          if (!p)
            {
              if (err_no_port)
                fatal(fmt("no port `" << net_name << "' in top-level module `"
                          << top->name() << "'"));
              else
                {
                  warning(fmt("no port `" << net_name << "' in top-level module `"
                              << top->name() << "', constraint ignored."));
                  continue;
                }
            }
          
          auto i = package.pin_loc.find(pin_name);
          if (i == package.pin_loc.end())
            fatal(fmt("unknown pin `" << pin_name << "' on package `"
                      << package.name << "'"));
          
          if (contains(net_pin_loc, net_name))
            fatal(fmt("duplicate pin constraints for net `" << net_name << "'"));
          
          const Location &loc = i->second;
          if (contains(pin_loc_net, loc))
            fatal(fmt("duplicate pin constraints for pin `" << pin_name <<"'"));
          
          extend(net_pin_loc, net_name, loc);
          extend(pin_loc_net, loc, net_name);
        }
      else
        fatal(fmt("unknown command `" << cmd << "'"));
    }
  
  constraints.net_pin_loc = net_pin_loc;
}

void
read_pcf(const std::string &filename, DesignState &ds)
{
  std::string expanded = expand_filename(filename);
  std::ifstream fs(expanded);
  if (fs.fail())
    fatal(fmt("read_pcf: failed to open `" << expanded << "': "
              << strerror(errno)));
  PCFParser parser(filename, fs, ds);
  return parser.parse();
}

class ConstraintsPlacer
{
  DesignState &ds;
  const ChipDB *chipdb;
  const Models &models;
  Model *top;
  const Constraints &constraints;
  
  BasedVector<Instance *, 1> cell_gate;
  
  Instance *top_port_io_gate(const std::string &net_name);
  
public:
  ConstraintsPlacer(DesignState &ds_);
  
  void place();
};

Instance *
ConstraintsPlacer::top_port_io_gate(const std::string &net_name)
{
  Port *p = top->find_port(net_name);
  assert(p);
  
  Port *p2 = p->connection_other_port();
  
  Instance *inst = cast<Instance>(p2->node());
  assert(models.is_ioX(inst)
         || models.is_pllX(inst));
  
  return inst;
}

ConstraintsPlacer::ConstraintsPlacer(DesignState &ds_)
  : ds(ds_),
    chipdb(ds.chipdb),
    models(ds.models),
    top(ds.top),
    constraints(ds.constraints),
    cell_gate(chipdb->n_cells, nullptr)
{
  assert(ds.placement.empty());
}

void
ConstraintsPlacer::place()
{
  std::vector<Net *> bank_latch(4, nullptr);
  for (const auto &p : constraints.net_pin_loc)
    {
      Instance *inst = top_port_io_gate(p.first);
      
      const Location &loc = p.second;
      int t = loc.tile();
      assert(chipdb->tile_type[t] == TileType::IO);
      int b = chipdb->tile_bank(t);
      
      int c = 0;
      if (models.is_ioX(inst))
        {
          Net *latch = inst->find_port("LATCH_INPUT_VALUE")->connection();
          if (latch)
            {
              if (bank_latch[b])
                {
                  if (bank_latch[b] != latch)
                    fatal(fmt("pcf error: multiple LATCH_INPUT_VALUE drivers in bank " << b));
                }
              else
                bank_latch[b] = latch;
            }
          
          if (inst->get_param("IO_STANDARD").as_string() == "SB_LVDS_INPUT"
              && b != 3)
            fatal(fmt("pcf error: LVDS port `" << p.first << "' not in bank 3\n"));
          
          Location loc_other(t,
                             loc.pos() ? 0 : 1);
          int cell_other = chipdb->loc_cell(loc_other);
          if (cell_other)
            {
              Instance *inst_other = cell_gate[cell_other];
              if (inst_other)
                {
                  int x = chipdb->tile_x(t),
                    y = chipdb->tile_y(t);
                  
                  if (inst->get_param("NEG_TRIGGER").get_bit(0)
                      != inst_other->get_param("NEG_TRIGGER").get_bit(0))
                    fatal(fmt("pcf error: incompatible NEG_TRIGGER parameters in PIO at (" 
                              << x << ", " << y << ")"));
                  
                  Net *cen = inst->find_port("CLOCK_ENABLE")->connection(),
                    *cen_other = inst_other->find_port("CLOCK_ENABLE")->connection();
                  if (cen && cen_other && cen != cen_other)
                    fatal(fmt("pcf error: multiple CLOCK_ENABLE drivers in PIO at ("
                              << x << ", " << y << ")"));
                  
                  Net *inclk = inst->find_port("INPUT_CLK")->connection(),
                    *inclk_other = inst_other->find_port("INPUT_CLK")->connection();
                  if (inclk && inclk_other && inclk != inclk_other)
                    fatal(fmt("pcf error: multiple INPUT_CLK drivers in PIO at ("
                              << x << ", " << y << ")"));
                  
                  Net *outclk = inst->find_port("OUTPUT_CLK")->connection(),
                    *outclk_other = inst_other->find_port("OUTPUT_CLK")->connection();
                  if (outclk && outclk_other && outclk != outclk_other)
                    fatal(fmt("pcf error: multiple OUTPUT_CLK drivers in PIO at ("
                              << x << ", " << y << ")"));
                }
            }
          
          c = chipdb->loc_cell(loc);
        }
      else
        {
          assert(models.is_pllX(inst));
          
          Location pll_loc(loc.tile(), 3);
          
          c = chipdb->loc_cell(pll_loc);
          if (!c
              || chipdb->cell_type[c] != CellType::PLL)
            fatal(fmt("bad constraint on `"
                      << p.first << "': no PLL at pin "
                      << ds.package.loc_pin.at(loc)));
        }
      
      cell_gate[c] = inst;
      extend(ds.placement, inst, c);
    }
  
  for (int c : chipdb->cell_type_cells[cell_type_idx(CellType::PLL)])
    {
      Instance *pll = cell_gate[c];
      if (!pll)
        continue;
      assert(models.is_pllX(pll));
      for (int io_cell : ds.pll_out_io_cells(pll, c))
        {
          Instance *io = cell_gate[io_cell];
          if (!io)
            continue;
          
          const BitVector &pin_type = io->get_param("PIN_TYPE").as_bits();
          if (io->find_port("D_IN_0")->connected()
              || io->find_port("D_IN_1")->connected()
              || !pin_type[0]
              || pin_type[1])
            {
              const Location &pll_loc = chipdb->cell_location[c];
              const std::string &io_pin = ds.package.loc_pin.at(chipdb->cell_location[io_cell]);
              fatal(fmt("PLL at `" << chipdb->tile_x(pll_loc.tile())
                        << " " << chipdb->tile_y(pll_loc.tile())
                        << "' conflicts with pin " << io_pin << " input path"));
            }
        }
    }
  
  int n_pll = 0,
    n_pll_placed = 0;
  for (Instance *inst : top->instances())
    {
      if (contains(ds.placement, inst))
        continue;
      
      // FIXME relax
      if (models.is_gb_io(inst))
        fatal("physical constraint required for GB_IO");
      else if (models.is_pllX(inst))
        {
          ++n_pll;
          
          bool good = false;
          for (int c : chipdb->cell_type_cells[cell_type_idx(CellType::PLL)])
            {
              if (cell_gate[c])
                continue;
              
              good = true;
              for (int io_cell : ds.pll_out_io_cells(inst, c))
                {
                  Instance *io = cell_gate[io_cell];
                  if (io)
                    {
                      const BitVector &pin_type = io->get_param("PIN_TYPE").as_bits();
                      if (io->find_port("D_IN_0")->connected()
                          || io->find_port("D_IN_1")->connected()
                          || !pin_type[0]
                          || pin_type[1])
                        {
                          good = false;
                          break;
                        }
                    }
                }
              if (good)
                {
                  cell_gate[c] = inst;
                  extend(ds.placement, inst, c);
                  ++n_pll_placed;
                  break;
                }
            }
          if (!good)
            fatal(fmt("failed to place: placed " << n_pll_placed
                      << " PLLs of " << n_pll
                      << " / " << chipdb->cell_type_cells[cell_type_idx(CellType::PLL)].size()));
        }
    }
}

void
place_constraints(DesignState &ds)
{
  ConstraintsPlacer placer(ds);
  placer.place();
}
