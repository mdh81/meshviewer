// Get ui elements that will be updated as emscripten runtime loads
var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var frameElement = window.frameElementent;

var urlSearchParameters = new URLSearchParams(window.location.search);

function hasParameter(name) {
    if (urlSearchParameters && urlSearchParameters.has(name))
        return true;
    else if (frameElement && frameElement.hasAttribute('data-'+name))
        return true;
    return false;
}

function getParameter(name) {
    if (urlSearchParameters && urlSearchParameters.has(name))
        return urlSearchParameters.get(name);
    else if (frameElement && frameElement.hasAttribute('data-'+name))
        return frameElement.getAttribute('data-'+name);
    return undefined;
}

function getFile(name) {
    var base = (window.location != window.parent.location) ? window.parent.location.href : window.location.href;
    var url  = new URL(name, base);
    var path = url.pathname;
    var file = path.split('/').pop();
    return [ file, path ];
}

function loadShaders(url) {
    return new Promise(function(resolve, reject) {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                if (xhr.status === 200) {
                    resolve(xhr.responseText);
                } else {
                    reject(new Error('Failed to load shaders from resource: ' + url));
                }
            }
        };
        xhr.open('GET', url, true);
        xhr.send();
    });
}

function extractShaderCode(html) {
    // Create a temporary div to parse the HTML content
    var tempDiv = document.createElement('div');
    tempDiv.innerHTML = html;

    // Extract script tags and their content
    var scriptTags = tempDiv.querySelectorAll('script[type^="x-shader"]');
    var shaderCodeMap = {};

    scriptTags.forEach(function(scriptTag) {
        var shaderId = scriptTag.getAttribute('id');
        var shaderCode = scriptTag.textContent;
        shaderCodeMap[shaderId] = shaderCode;
    });

    return shaderCodeMap;
}

var argv = [];

// Emscripten Module object
var Module = {

    preRun: [ function(){

        // convert shaders to DOM objects so the c++ code get hold of them at runtime
        loadShaders('shaders/shaders.html')
            .then(function(html) {
                var shaderCodeMap = extractShaderCode(html);

                Object.keys(shaderCodeMap).forEach(function(shaderId) {
                    var scriptTag = document.createElement('script');
                    scriptTag.id = shaderId;
                    scriptTag.type = "x-shader";
                    scriptTag.textContent = shaderCodeMap[shaderId];
                    document.head.appendChild(scriptTag);
                });
            })
            .catch(function(error) {
                console.error(error);
            });

        // model to be loaded
        if (hasParameter('model')) {
            var [filename, pathname ] = getFile(getParameter('model'));
            FS.createPreloadedFile('./', filename, pathname, true, false);
            argv.push(filename);
        }

    }],

    arguments: argv,

    postRun: [],

    print: function(text) {
        console.log(text);
    },

    printErr: function(text) {
        console.error(text);
    },

    canvas: (function() {
        var canvas = document.getElementById('canvas');

        // As a default initial behavior, pop up an alert when webgl context is lost. To make your
        // application robust, you may want to override this behavior before shipping!
        // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
        canvas.addEventListener('webglcontextlost', function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

        return canvas;
    })(),

    setStatus: function(text) {
        if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
        if (text === Module.setStatus.text) return;
        var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
        var now = Date.now();
        if (m && now - Date.now() < 30) return; // if this is a progress update, skip it if too soon
        if (m) {
            text = m[1];
            progressElement.value = parseInt(m[2])*100;
            progressElement.max = parseInt(m[4])*100;
            progressElement.hidden = false;
        } else {
            progressElement.value = null;
            progressElement.max = null;
            progressElement.hidden = true;
        }
        statusElement.innerHTML = text;
    },

    totalDependencies: 0,

    monitorRunDependencies: function(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    }

};

Module.setStatus('Downloading...');
window.onerror = function() {
    Module.setStatus('Exception thrown, see JavaScript console');
    Module.setStatus = function(text) {
        if (text) Module.printErr('[post-exception status] ' + text);
    };
};