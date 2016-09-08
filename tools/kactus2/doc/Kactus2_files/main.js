
var sequences;
var ACTIVE_CLASS = "btn-primary";
var DEACTIVE_CLASS = "btn-default";
var DOWNLOAD_BASE = "video/";
var FILTER_NO_SEQUENCES = "No sequences matching current filter";

var toggle_display ={
                      "1920x1080":"1080p",
                      "3840x2160": "4K",
                      "8bit": "8",
                      "10bit": "10",
                      "bitdepth": "Bit Depth",
                      "format": "Format",
                      "resolution": "Resolution",
                      "global": "Versions"
                    };

var toggle_array = {"global": ["All"],
                    "resolution":["1920x1080","3840x2160"],
                    //"fps":["30fps","120fps"],
                    //"subsampling":["420"],
                    "bitdepth":["8bit","10bit"],
                    "format":["AVC", "HEVC", "YUV"]
                    };

function build_row(val1, val2, val3, w1, w2, w3)
{
 var data = "<tr>";
 //First column
 data+="<td "+((val2=="")?"colspan=\"3\" ":"")+"style=\"width: "+w1+"px;\" valign=\"top\">"+val1+"</td>";
 //Second column
 if(val2 != "") data+="<td "+((val3=="")?"colspan=\"2\" ":"valign=\"top\" ")+"style=\"width: "+w2+"%;\">"+val2+"</td>";
 //Third column
 //if(val3 != "") data+="<td valign=\"bottom\" style=\"width: "+w3+"px;\">"+val3+"</td>";
 data+="</tr>"; 
 
 return data;
}

function toggle_filter_activate_one(seq,group,filter)
{
  if(group == "global" && filter == "All")
  {
    $('#'+seq+"_toggle_"+group+"_"+filter).removeClass(ACTIVE_CLASS);
    $('#'+seq+"_toggle_"+group+"_"+filter).addClass(DEACTIVE_CLASS);
  }
  else
  {
    $.each(toggle_array[group],function(key,value)
    {
      $('#'+seq+"_toggle_"+group+"_"+value).removeClass(ACTIVE_CLASS);
      $('#'+seq+"_toggle_"+group+"_"+value).removeClass(DEACTIVE_CLASS);
      if(value == filter) $('#'+seq+"_toggle_"+group+"_"+value).addClass(ACTIVE_CLASS);
      else $('#'+seq+"_toggle_"+group+"_"+value).addClass(DEACTIVE_CLASS);    
    });
  }
  filter_array(seq);
}

function toggle_filter(seq,group,filter)
{
  $('#'+seq+"_toggle_"+group+"_"+filter).toggleClass(ACTIVE_CLASS);
  $('#'+seq+"_toggle_"+group+"_"+filter).toggleClass(DEACTIVE_CLASS);
  
  if(filter != "All")
  {
    if($('#'+seq+"_toggle_global_All").hasClass(ACTIVE_CLASS))
    {
      $('#'+seq+"_toggle_global_All").removeClass(ACTIVE_CLASS);
      $('#'+seq+"_toggle_global_All").addClass(DEACTIVE_CLASS);
    }
  }
  
  if(group == "global" && filter == "All" && $('#'+seq+"_toggle_global_All").hasClass(ACTIVE_CLASS))
  {
    $.each(toggle_array,function(key,value)
    {
     $.each(toggle_array[key],function(key2,value2)
     {
      if(value2 != "All")
      {
        $('#'+seq+"_toggle_"+key+"_"+value2).removeClass(DEACTIVE_CLASS);
        $('#'+seq+"_toggle_"+key+"_"+value2).addClass(ACTIVE_CLASS);
      }     
      });
     });
  }
  
  
  filter_array(seq);
}

function filter_array(seq)
{
  var data = "";
  for (var i = 0; i < sequences.length; i++)
  {
    if(sequences[i].name != seq) continue;
    $.each(sequences[i].downloads, function(key3, value3) {
      var select_this = 1;
      
      $.each(toggle_array, function(key,value)
      {
        var select_this_group = 0;
        if(key != "global")
        {
          $.each(value, function(filter_key, filter_value)
          {
            if($('#'+seq+"_toggle_"+key+"_"+filter_value).hasClass(ACTIVE_CLASS) && 
               key3.indexOf(filter_value) != -1)
            {
              select_this_group = 1;
            }
          });
        }
        else
        {
          select_this_group = 1;
        }
        if(!select_this_group) select_this = 0;
      });
      var split_name = key3.split(".");
      split_name = split_name[0].split("_");
      if(select_this || $('#'+seq+"_toggle_global_All").hasClass(ACTIVE_CLASS))
      {
        data+="<span style=\"font-size: 12px; font-weight: bold;\"><a href=\""+DOWNLOAD_BASE+key3+"\">Download "+split_name[1]+" "+split_name[2]+" "+split_name[3]+" "+split_name[4]+" "+split_name[5]+"</a> ("+(value3/(1024*1024)).toFixed(2)+" MiB)</span><br />";
      }
      
    });
    break;
  }
  if(data == "") data = FILTER_NO_SEQUENCES;
  $('#'+seq+'_downloads').html(data);
}

function search_filter(seq, filter)
{
  var this_filter = filter;
  var match_seq = 0;
  var key3, value3;
  for (var i = 0; i < sequences.length; i++)
  {
    if(sequences[i].name != seq) continue;
    $.each(sequences[i].downloads, function(key3, value3) {
      //alert(key3+" "+this_filter);
      if(key3.indexOf(this_filter) != -1)
      {        
        match_seq++;
      }
    });
    break;
  }
  return match_seq;
}


function filter_link_builder(seq)
{
  var data = "<div style=\"display: inline-block;\">";
  var key, value;
  $.each(toggle_array, function(key, value) {
    var toggles = 0;
    var data_temp = "";
    var key2,value2;
    var last = "";
    data_temp+="<div style=\"display: block;\"><span style=\"display: block; width: 100px; float: left;\">";
    data_temp+=(toggle_display[key]?toggle_display[key]:key)+":</span> ";
    $.each(toggle_array[key], function(key2,value2) {
      if(search_filter(seq,value2) || key == "global")
      {
        var button_value = toggle_display[value2]?toggle_display[value2]:value2;
        data_temp+="<a id='"+seq+"_toggle_"+key+"_"+value2+"' class='filter btn btn-xs btn-primary' onClick=\"toggle_filter('"+seq+"','"+key+"','"+value2+"');\">"+button_value+"</a>";        
        toggles++;
      }
    });
    if(toggles)
    {
      data+=data_temp;
      data+="</div>";
    }
  });
  data+="</div>"
  return data;
}

// Read Sequence JSON
$(document).ready(function() {

    var PER_ROW = 3;
    

    $.getJSON('sequence_data.json', function(data) {
        sequences = data.sequences;
        var container = $('#sequence_data');
        var html_data = "";
        for (var i = 0; i < sequences.length; i++)
        {
          if(i%PER_ROW == 0) 
          {
            if(i != 0) html_data += "</div>";
            html_data += "<div class='row'>";
          }
          html_data += "<div class='col-lg-4 col-blue'>";          
          html_data += "<h2>"+sequences[i].name+"</h2>";
          html_data+="<img class='img-autoscale' src='img/"+sequences[i].thumbnail+"'>";          
          html_data += "<p><a class='btn btn-xs btn-info' data-toggle='collapse' data-target='#"+sequences[i].name+"'>Details <b class='caret'></b></a></p>";          
          html_data += "<div class='collapse' id='"+sequences[i].name+"'>";
          html_data +="<table>";
          //<b>Name</b>:"+sequences[i].name+"<br />";,
          html_data += build_row("<b>Date</b>:",sequences[i].date,"",105,90,100);
          html_data += build_row("<b>Length</b>:",sequences[i].len,"",105,90,100);
          html_data += build_row("<b>Camera</b>:",sequences[i].camera,"",105,90,100);
          html_data += build_row("<b>Description</b>:",sequences[i].description,"",105,90,100);
          
          var first = 1;
          $.each(sequences[i].original, function(key, value) {
          /*
              if(first)
              {
                html_data += build_row("<b>Original Material</b>:","","");
                first = 0;
              }
              */
              html_data += build_row("<b>"+key+"</b>",value, "",105,90,100);         
          });
          
          $.each(sequences[i].other, function(key, value) {
              if(key != "") html_data += build_row("<b>"+key+"</b>:",value,"",105,90,100);
              else html_data += build_row("",value,"",105,90,100);              
          });
          html_data += "</table>";
          html_data += "</div>";
          
          html_data+="<b>Download</b> <br/>";
          html_data+=filter_link_builder(sequences[i].name);
          html_data+="<br />";
          html_data+="<div id=\""+sequences[i].name+"_downloads\"></div>";
          /*
          $.each(sequences[i].downloads, function(key, value) {
              //html_data += "<a class='btn btn-xs btn-primary' href='"+value+"'>"+key+"</a> ";
          });
          */
          html_data += "</div>";
        }
        html_data += "</div>";
        container.html(html_data);
        
        //Reset to default filter
        for (var i = 0; i < sequences.length; i++)
        {
          var key,value;
          $.each(sequences[i].downloads, function(key, value)
          {
            var last = "";
            var key2,value2;
            $.each(toggle_array, function(key2,value2)
            {
              var filters = 0;
              var key3,value3;
              $.each(value2, function(key3,value3)
              {
                if(search_filter(sequences[i].name,value3))
                {
                  last = value3;
                  filters++;
                }                
              });
              if(filters > 1 || key2 == "global") toggle_filter_activate_one(sequences[i].name,key2,last);
            });

            
          });
        }
        
        
    });

});
