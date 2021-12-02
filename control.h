const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><title>BTL IOT n6</title>
<style>
.color-picker-container {
  display: inline-block;
  background: #00A8A9 none repeat scroll 0% 0%;
  border-radius: 4px;
  border: 2px solid #f8fafb;
}
.color-picker-container .picker-container .canvas-container {
  margin: 20px;
  position: relative;
  float: left;
  width: 200px;
  display: inline-block;
  background: #00A8A9;
}
.color-picker-container .picker-container .canvas-container.active {
  display: block;
}
.color-picker-container .picker-container .canvas-container canvas {
  cursor: crosshair;
  border-radius: 50%;
  box-shadow: 0 0 0 4px #E8E8E8;
  background: #E6D3D3;
}
.color-picker-container .picker-container .canvas-container .pointer {
  width: 15px;
  height: 15px;
  border: 2px solid #fff;
  border-radius: 50%;
  position: absolute;
  pointer-events: none;
  background: rgba(0, 0, 0, 0.1);
}
.color-picker-container .picker-container .canvas-container input {
  margin-top: 10px;
  width: 100%;
  height: 30px;
  text-align: center;
  background: #353738;
  border: 0;
  color: #fff;
}
.color-picker-container .picker-container .slider-container {
  width: 15px;
  float: right;
  position: relative;
  margin: 15px;
}
.color-picker-container .picker-container .slider-container .slider {
  width: 15px;
  height: 249px;
  background: #000;
}
.color-picker-container .picker-container .slider-container .pointer {
  width: 0;
  height: 0;
  border-style: solid;
  border-width: 5px 0 5px 10px;
  border-color: transparent transparent transparent #ffffff;
  position: absolute;
  left: -8px;
}
.color-picker-container .palletes-container {
  float: right;
  width: 275px;
}
.color-picker-container .palletes-container .palette {
  width: 35px;
  height: 35px;
  float: right;
  border-radius: 4px;
  margin: 5px;
  box-shadow: inset 0px 2px 1px rgba(0, 0, 0, 0.28);
  cursor: pointer;
}
.color-picker-container .palletes-container .palette.active {
  box-shadow: 0 0 0 3px #3F3F40;
}
.color-picker-container .palletes-container .palette.add {
  border: 2px dashed #bababa;
  box-shadow: inherit;
  position: relative;
}
.color-picker-container .palletes-container .palette.add:after {
  content: '+';
  font-size: 24px;
  color: #bababa;
  position: absolute;
  width: 100%;
  height: 100%;
  left: 0;
  top: 0;
  text-align: center;
  line-height: 30px;
}
  .slider {

-webkit-appearance: none;

width: 50%;

height: 15px;

border-radius: 5px;

background: orange;

outline: none;

opacity: 0.5;

-webkit-transition: .2s;

transition: opacity .2s;

}


.slider:hover {

opacity: 2;

}


.slider::-webkit-slider-thumb {

-webkit-appearance: none;

appearance: none;

width: 25px;

height: 25px;

border-radius: 50%;

background: brown;

cursor: pointer;

}


.slider::-moz-range-thumb {

width: 25px;

height: 25px;

border-radius: 50%;

background: #4CAF50;

cursor: pointer;

}
</style>
</head>
<body>
<h1>NodeMCU ESP8266 Web Server</h1>
<h2>BTL Internet of Things<h2>
<h2>Author: Nguyen Truong Son<h2>
<div id="picker"></div>
<script>
 var ColorPicker;

 (function() {

   function insertBefore(element, before) {
     parent = before.parentNode;
     parent.insertBefore(element, before);
   }

   function extend(defaults, options) {
     var extended = {};
     var prop;
     for (prop in defaults) {
       if (Object.prototype.hasOwnProperty.call(defaults, prop)) {
         extended[prop] = defaults[prop];
       }
     }
     for (prop in options) {
       if (Object.prototype.hasOwnProperty.call(options, prop)) {
         extended[prop] = options[prop];
       }
     }
     return extended;
   };

   function hasClass(element, classname) {
     var className = " " + classname + " ";
     if ((" " + element.className + " ").replace(/[\n\t]/g, " ").indexOf(" " + classname + " ") > -1) {
       return true;
     }
     return false;
   }

   function removeClass(node, className) {
     node.className = node.className.replace(
       new RegExp('(^|\\s+)' + className + '(\\s+|$)', 'g'),
       '$1'
     ).replace(/ +(?= )/g, '').trim();
   }

   function addClass(element, className) {
     if (!hasClass(element, className)) {
       element.className += ' ' + className;
       element.className = element.className.replace(/ +(?= )/g, '').trim()
     }
   }

   ColorPicker = function(element, options) {

     this.options = extend({
       color: '#e7e7e7',
       palettes: ['#646fff', '#fffa1d', '#ffa21f', '#ff391d'],
       onUpdate: function() {}
     }, options);

     this.options.palettes.unshift(this.options.color);

     this.hex = this.options.color;
     this.rgb = this.HEXtoRGB(this.hex);
     this.hsv = this.RGBtoHSV(this.rgb[0], this.rgb[1], this.rgb[2]);
     this.dom = {};
     this.dom.container = document.createElement('div');
     this.dom.container.className = 'color-picker-container';

     element.appendChild(this.dom.container);

     this.initPicker();

     this.initPalettes();
   }

   ColorPicker.prototype.initPicker = function() {

     this.dom.picker = {};
     this.dom.picker.container = document.createElement('div');
     this.dom.picker.container.className = 'picker-container';

     this.dom.container.appendChild(this.dom.picker.container);

     this.dom.picker.canvas = {};

     this.dom.picker.canvas.container = document.createElement('div');
     this.dom.picker.canvas.container.className = 'canvas-container';
     this.dom.picker.container.appendChild(this.dom.picker.canvas.container);

     this.dom.picker.canvas.canvas = document.createElement('canvas');
     this.dom.picker.canvas.canvas.className = 'canvas';

     this.dom.picker.canvas.pointer = document.createElement('div');
     this.dom.picker.canvas.pointer.className = 'pointer';

     var ctx = this.dom.picker.canvas.canvas.getContext('2d'),
       image = new Image,
       $this = this,
       dragging = false;

     this.dom.picker.canvas.canvas.setAttribute('width', 200);
     this.dom.picker.canvas.canvas.setAttribute('height', 200);
     this.dom.picker.canvas.container.appendChild(this.dom.picker.canvas.canvas);
     this.dom.picker.canvas.container.appendChild(this.dom.picker.canvas.pointer);

     // image.src = 'images/wheel copy.png';
     image.src = 'data:image/png;base64,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';

     image.onload = function() {
       $this.updateCanvasBounds();
       ctx.drawImage(image, 0, 0, 200, 200);
       $this.updateCoordinates($this.dom.picker.canvas.canvas.bounds.centerX, $this.dom.picker.canvas.canvas.bounds.centerY);
       coordinates = $this.getPositionFromColor($this.hex);
       if (coordinates != null) {
         $this.x = coordinates.x;
         $this.y = coordinates.y;
         $this.updateColor($this.HEXtoRGB($this.hex));
         $this.updateAll();
       }
       $this.options.onUpdate($this.rgb);
     };

     this.dom.picker.canvas.canvas.addEventListener('mousedown', function(e) { 
       e.preventDefault();
       dragging = true;
       $this.updateCoordinates(e.clientX, e.clientY);
       imageData = ctx.getImageData($this.x, $this.y, 1, 1);
       $this.updateColor(imageData.data);
       $this.hsv[2] = 1;
       $this.updateAll();
     });

     document.addEventListener('mousemove', function(e) { // mouse move handler
       if (dragging) {
         $this.updateCoordinates(e.pageX, e.pageY);
         imageData = ctx.getImageData($this.x, $this.y, 1, 1);
         $this.updateColor(imageData.data);
         $this.hsv[2] = 1;
         $this.updateAll();
       }
     });

     document.addEventListener('mouseup', function(e) { // click event handler
       dragging = false;
     });

     this.dom.picker.canvas.input = document.createElement('input'),

       this.dom.picker.canvas.container.appendChild(this.dom.picker.canvas.input);

     this.dom.picker.canvas.input.addEventListener('keyup', function() {
       if(this.value == $this.hex || '#' + this.value == $this.hex){
         return;
       }
       coordinates = $this.getPositionFromColor(this.value);
       if (coordinates != null) {
         $this.x = coordinates.x;
         $this.y = coordinates.y;
         $this.updateColor($this.HEXtoRGB(this.value));
         $this.updateAll();
       }
     });

     this.initSlider();

   }

   ColorPicker.prototype.initSlider = function() {

     this.dom.slider = {};
     this.dom.slider.container = document.createElement('div');
     this.dom.slider.container.className = 'slider-container';

     this.dom.slider.slider = document.createElement('div');
     this.dom.slider.slider.className = 'slider';

     this.dom.slider.pointer = document.createElement('div');
     this.dom.slider.pointer.className = 'pointer';

     this.dom.slider.container.appendChild(this.dom.slider.pointer);
     this.dom.slider.container.appendChild(this.dom.slider.slider);
     this.dom.picker.container.appendChild(this.dom.slider.container);

     this.dom.slider.slider.bounds = this.dom.slider.slider.getBoundingClientRect();
     this.dom.slider.pointer.bounds = this.dom.slider.pointer.getBoundingClientRect();

     this.redrawSlider();

     var dragging = false,
       $this = this;

     this.dom.slider.slider.addEventListener('mousedown', function(e) {
       e.preventDefault();
       dragging = true;
       total = $this.updateSliderCursor(e.clientY);
       $this.updateColor($this.HSVtoRGB($this.hsv[0], $this.hsv[1], 1 - total));
       $this.updateAll();
     });

     this.dom.slider.pointer.addEventListener('mousedown', function(e) {
       e.preventDefault();
       dragging = true;
       total = $this.updateSliderCursor(e.clientY);
       $this.updateColor($this.HSVtoRGB($this.hsv[0], $this.hsv[1], 1 - total));
       $this.updateAll();
     });

     document.addEventListener('mousemove', function(e) {
       if (!dragging) {
         return;
       }
       total = $this.updateSliderCursor(e.clientY);
       $this.updateColor($this.HSVtoRGB($this.hsv[0], $this.hsv[1], 1 - total));
       $this.updateAll();
     });

     document.addEventListener('mouseup', function() {
       dragging = false;
     });
   };

   ColorPicker.prototype.updateColor = function(pixel) {
     this.hex = hex = this.RGBtoHEX(pixel[0], pixel[1], pixel[2]);
     this.hsv = this.RGBtoHSV(pixel[0], pixel[1], pixel[2]);
     this.rgb = [
       pixel[0],
       pixel[1],
       pixel[2]
     ];
   }

   ColorPicker.prototype.updateCoordinates = function(x, y) {
     var angle = Math.atan2((y - this.dom.picker.canvas.canvas.bounds.centerY), (x - this.dom.picker.canvas.canvas.bounds.centerX));
     radius = Math.sqrt(Math.pow(x - this.dom.picker.canvas.canvas.bounds.centerX, 2) + Math.pow(y - this.dom.picker.canvas.canvas.bounds.centerY, 2));
     if (radius > this.dom.picker.canvas.canvas.bounds.radius - (this.dom.picker.canvas.pointer.bounds.width / 2)) {
       cos = Math.cos(angle);
       sin = Math.sin(angle);
       x = cos * (this.dom.picker.canvas.canvas.bounds.radius - (this.dom.picker.canvas.pointer.bounds.width / 2)) + this.dom.picker.canvas.canvas.bounds.centerX;
       y = sin * (this.dom.picker.canvas.canvas.bounds.radius - (this.dom.picker.canvas.pointer.bounds.width / 2)) + this.dom.picker.canvas.canvas.bounds.centerY;
     }
     this.x = Math.floor(x - this.dom.picker.canvas.canvas.bounds.left);
     this.y = Math.floor(y - this.dom.picker.canvas.canvas.bounds.top);
   }

   ColorPicker.prototype.initPalettes = function() {
     this.dom.palettes = {};
     this.dom.palettes.list = [];
     this.dom.palettes.container = document.createElement('div');
     addClass(this.dom.palettes.container, 'palletes-container');
     this.dom.container.appendChild(this.dom.palettes.container);
     this.dom.palettes.add = document.createElement('div');
     addClass(this.dom.palettes.add, 'palette add');
     this.dom.palettes.container.appendChild(this.dom.palettes.add);
     var $this = this;
     this.dom.palettes.add.addEventListener('click', function() {
       addClass($this.dom.picker.canvas.container, 'active');
       $this.updateCanvasBounds();
       palette = $this.addPalette($this.RGBtoHEX($this.rgb[0], $this.rgb[1], $this.rgb[2]));
       for (var i = 0; i < $this.dom.palettes.list.length; i++) {
         removeClass($this.dom.palettes.list[i], 'active');
       }
       addClass(palette, 'active');
       $this.selectedPalette = palette;
     });
     for (var i = 0; i < this.options.palettes.length; i++) {
       this.addPalette(this.options.palettes[i]);
     }
   }

   ColorPicker.prototype.addPalette = function(color) {
     var palette = document.createElement('div');
     palette.style.background = color;
     palette.color = color;
     var $this = this;
     palette.addEventListener('click', function() {
       for (var i = 0; i < $this.dom.palettes.list.length; i++) {
         removeClass($this.dom.palettes.list[i], 'active');
       }
       addClass(this, 'active');
       $this.selectedPalette = this;
       rgb = $this.HEXtoRGB(this.color);
       coordinates = $this.getPositionFromColor(color);
       $this.x = coordinates.x;
       $this.y = coordinates.y;
       $this.updateColor(rgb);
       $this.updateAll();
     });
     addClass(palette, 'palette');
     insertBefore(palette, this.dom.palettes.add);
     this.dom.palettes.list.push(palette);
     return palette;
   }

   ColorPicker.prototype.updateAll = function() {
     this.redrawSlider();
     this.updatePointers();
     this.dom.picker.canvas.input.value = this.hex;
     this.options.onUpdate(this.rgb);
     if (this.selectedPalette) {
       this.selectedPalette.style.background = this.hex;
     }
   }
   ColorPicker.prototype.getPositionFromColor = function(color) {
     color = this.HEXtoRGB(color);
     if (color == null) {
       return null;
     }
     this.hsv = this.RGBtoHSV(color[0], color[1], color[2]);
     return this.getSVGPositionFromHS(this.hsv[0], this.hsv[1]);
   }

   ColorPicker.prototype.updateSliderCursor = function(y) {
     total = y - this.dom.slider.slider.bounds.top - 6;
     total = this.dom.slider.slider.bounds.height - total;
     total = total / this.dom.slider.slider.bounds.height;
     total = total.toFixed(2);
     if (total < 0) {
       total = 0;
     } else if (total > 1) {
       total = 1;
     }
     total = 1 - total;
     this.dom.slider.pointer.style.top = this.dom.slider.slider.bounds.height * total - (this.dom.slider.pointer.bounds.height / 2) + 'px';
     return total;
   }

   ColorPicker.prototype.redrawSlider = function() {
     rgb = this.HSVtoRGB(this.hsv[0], this.hsv[1], 1);
     hex = this.RGBtoHEX(rgb[0], rgb[1], rgb[2]);
     gradient = this.makeGradient(hex, '#000');
     this.dom.slider.slider.setAttribute('style', gradient);
     this.updatePointers();
   };

   ColorPicker.prototype.updatePointers = function() {
     if (this.dom.picker.canvas.pointer.bounds) {
       this.dom.picker.canvas.pointer.style.left = this.x - (this.dom.picker.canvas.pointer.bounds.width / 2) + 'px';
       this.dom.picker.canvas.pointer.style.top = this.y - (this.dom.picker.canvas.pointer.bounds.height / 2) + 'px';
     }
     if (this.dom.slider.slider.bounds) {
       position = this.dom.slider.slider.bounds.height * (1 - this.hsv[2]) - (this.dom.slider.pointer.bounds.height / 2);
       this.dom.slider.pointer.style.top = position + 'px';
     }
   }

   ColorPicker.prototype.updateCanvasBounds = function() {
       this.dom.picker.canvas.canvas.bounds = this.dom.picker.canvas.canvas.getBoundingClientRect();
       this.dom.picker.canvas.pointer.bounds = this.dom.picker.canvas.pointer.getBoundingClientRect();
       this.dom.picker.canvas.canvas.bounds.centerX = this.dom.picker.canvas.canvas.bounds.left + (this.dom.picker.canvas.canvas.bounds.width / 2);
       this.dom.picker.canvas.canvas.bounds.centerY = this.dom.picker.canvas.canvas.bounds.top + (this.dom.picker.canvas.canvas.bounds.height / 2);
       this.dom.picker.canvas.canvas.bounds.radius = this.dom.picker.canvas.canvas.bounds.width / 2;
     }
     // https://codepen.io/benknight/pen/nADpy
     // Get a coordinate pair from hue and saturation components.
   ColorPicker.prototype.getSVGPositionFromHS = function(h, s) {
     var hue = this.scientificToArtisticSmooth(h * 360);
     var theta = hue * (Math.PI / 180);
     var y = Math.sin(theta) * this.dom.picker.canvas.canvas.bounds.radius * s;
     var x = Math.cos(theta) * this.dom.picker.canvas.canvas.bounds.radius * s;
     return {
       x: x + this.dom.picker.canvas.canvas.bounds.radius,
       y: this.dom.picker.canvas.canvas.bounds.radius - y
     }

   };

   //https://codepen.io/benknight/pen/nADpy
   ColorPicker.prototype.scientificToArtisticSmooth = function(hue) {
     return (
       hue < 35 ? hue * (60 / 35) :
       hue < 60 ? this.mapRange(hue, 35, 60, 60, 122) :
       hue < 120 ? this.mapRange(hue, 60, 120, 122, 165) :
       hue < 180 ? this.mapRange(hue, 120, 180, 165, 218) :
       hue < 240 ? this.mapRange(hue, 180, 240, 218, 275) :
       hue < 300 ? this.mapRange(hue, 240, 300, 275, 330) :
       this.mapRange(hue, 300, 360, 330, 360));
   }

   //https://codepen.io/benknight/pen/nADpy
   ColorPicker.prototype.mapRange = function(value, fromLower, fromUpper, toLower, toUpper) {
     return (toLower + (value - fromLower) * ((toUpper - toLower) / (fromUpper - fromLower)));
   }

   //https://gist.github.com/Arahnoid/9923989
   ColorPicker.prototype.HEXtoRGB = function(hex) {
     var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
     return result ? [
       parseInt(result[1], 16),
       parseInt(result[2], 16),
       parseInt(result[3], 16)
     ] : null;
   }

   //http://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c
   ColorPicker.prototype.RGBtoHSV = function(r, g, b) {
     r = r / 255, g = g / 255, b = b / 255;
     var max = Math.max(r, g, b),
       min = Math.min(r, g, b);
     var h, s, v = max;

     var d = max - min;
     s = max == 0 ? 0 : d / max;

     if (max == min) {
       h = 0; // achromatic
     } else {
       switch (max) {
         case r:
           h = (g - b) / d + (g < b ? 6 : 0);
           break;
         case g:
           h = (b - r) / d + 2;
           break;
         case b:
           h = (r - g) / d + 4;
           break;
       }
       h /= 6;
     }
     return [h, s, v];
   }

   //http://axonflux.com/handy-rgb-to-hsl-and-rgb-to-hsv-color-model-c
   ColorPicker.prototype.HSVtoRGB = function(h, s, v) {
     var r, g, b;
     var i = Math.floor(h * 6);
     var f = h * 6 - i;
     var p = v * (1 - s);
     var q = v * (1 - f * s);
     var t = v * (1 - (1 - f) * s);
     switch (i % 6) {
       case 0:
         r = v, g = t, b = p;
         break;
       case 1:
         r = q, g = v, b = p;
         break;
       case 2:
         r = p, g = v, b = t;
         break;
       case 3:
         r = p, g = q, b = v;
         break;
       case 4:
         r = t, g = p, b = v;
         break;
       case 5:
         r = v, g = p, b = q;
         break;
     }
     return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
   }

   //https://gist.github.com/Arahnoid/9923989
   ColorPicker.prototype.RGBtoHEX = function(r, g, b) {
     function componentToHex(c) {
       var hex = c.toString(16);
       return hex.length == 1 ? "0" + hex : hex;
     }
     return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
   }

   //http://jsfiddle.net/barney/D9W4v/
   ColorPicker.prototype.makeGradient = function(colour1, colour2) {
     var gradientString = '\
            /* Mozilla Firefox */ \
            background-image: -moz-linear-gradient(top, {colour1} 0%, {colour2} 100%);\
            /* Opera */ \
            background-image: -o-linear-gradient(top, {colour1} 0%, {colour2} 100%);\
            /* Webkit (Safari/Chrome 10) */ \
            background-image: -webkit-gradient(linear, left top, left bottom, color-stop(0, {colour1}), color-stop(1, {colour2}));\
            /* Webkit (Chrome 11+) */ \
            background-image: -webkit-linear-gradient(top, {colour1} 0%, {colour2} 100%);\
            /* IE10+ */\
            background: -ms-linear-gradient(top,  {colour1} 0%,{colour2} 100%);\
            /* W3C */\
            background: linear-gradient(top,  {colour1} 0%,{colour2} 100%);\
        ';

     return gradientString.replace(/\{colour1\}/g, colour1).replace(/\{colour2\}/g, colour2)
   };
 }());

 var picker = new ColorPicker(document.getElementById('picker'), {
   onUpdate: function(rgb) {
     document.body.style.background = "rgb(" + rgb[0] + ", " + rgb[1] + ", " + rgb[2] + ")";
     changeColor(rgb[0],rgb[1],rgb[2]);
   }
 });

//setInterval(changeColor, 2000);

function changeColor(red, green, blue){
   //Set RGB LED Color
   console.log(red,green,blue);
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
  var txt = this.responseText;
      }  
    };
   xhttp.open("GET", "setRGB?r="+red+"&g="+green+"&b="+blue, true); //Handle readADC server on ESP8266
   xhttp.send();
} 
</script>
<div class="Servo">

    <br><br>

    <div class="slidecontainer">

      <input type="range" min="0" max="180" value="50" class="slider" id="myRange">

      <p>Servo angle: <span id="demo"></span></p>

    </div>

    <p><button onClick="onFeed()">Click to feed</button></p>
    <p><button onClick="onTurnoff()">Turn light off</button></p>
    <p><a href='/temp'><button>Click to see the temperature</button></a></p>

    <script>
      function onTurnoff() {

        var xhttp = new XMLHttpRequest();

        xhttp.onreadystatechange = function() {

          if (this.readyState == 4 && this.status == 200) {

            console.log(this.responseText);

          }

        };

        xhttp.open("GET", "turnoff", true);

        xhttp.send();

      } 
      function onFeed() {

        var xhttp = new XMLHttpRequest();

        xhttp.onreadystatechange = function() {

          if (this.readyState == 4 && this.status == 200) {

            console.log(this.responseText);

          }

        };

        xhttp.open("GET", "feed", true);

        xhttp.send();

      } 

      function sendData(pos) {

        var xhttp = new XMLHttpRequest();

        xhttp.onreadystatechange = function() {

          if (this.readyState == 4 && this.status == 200) {

            console.log(this.responseText);

          }

        };

        xhttp.open("GET", "setPOS?servoPOS="+pos, true);

        xhttp.send();

      } 

      var slider = document.getElementById("myRange");

      var output = document.getElementById("demo");

      output.innerHTML = slider.value;


      slider.oninput = function() {

        output.innerHTML = this.value;

        sendData(output.innerHTML);

      }

    </script>
</div>
</body>
</html>
)=====";
