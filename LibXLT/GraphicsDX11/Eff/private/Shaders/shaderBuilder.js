var proc = require("child_process");
var fs = require("fs");

var fxcpath = "\"C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x64\\fxc.exe\"";

var sh = "D:\\Projects\\SourceCode\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\";

// preprocess shader
proc.execSync(fxcpath + " /P out.fx " + sh + "Blinn.fx");

proc.execSync(fxcpath + " /Fc out2.fx /T ps_5_0 /E projLightPS_Default " + sh + "Blinn.fx");
proc.execSync(fxcpath + " /Fc out3.fx /T fx_5_0 " + sh + "Blinn.fx");


var fileString = fs.readFileSync("out.fx", {encoding:"utf8"});

var firstTech = fileString.indexOf("technique11");
if (firstTech < 0) {
  return;
}
fileString = fileString.substr(firstTech);
//fs.writeFileSync("temp.fx", fileString);



var words = fileString.match(/\w+/g);
var myFX = [];
var technique;
var pass;
for (var i = 0; i < words.length; ++i) {
  var token = words[i];

  if (token === "technique11" || token === "technique") {
    technique = {name:words[i+1], passes:[]};
    myFX.push(technique);
  }
  else if (token === "pass") {
    pass = {name:words[i+1]};
    technique.passes.push(pass);
  }
  else if (token === "ps_5_0") {
    pass.ps = words[i+1];
  }
  else if (token === "vs_5_0") {
    pass.vs = words[i+1];
  }
  else if (token === "hs_5_0") {
    pass.hs = words[i+1];
  }
  else if (token === "ds_5_0") {
    pass.ds = words[i+1];
  }
  else if (token === "gs_5_0") {
    pass.gs = words[i+1];
  }
}
console.log(JSON.stringify(myFX));

// var fileStringArray = fileString.split('\n');
// var fileOutArray = [];
// for (var i = 0; i < fileStringArray.length; ++i) {
//   if (fileStringArray[i].substr(0,6) !== "#line ") {
//     fileOutArray.push(fileStringArray[i]);
//   }
// }
// var fileOutStr = fileOutArray.join('\n');
