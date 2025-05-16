// WebGL Room Shader Port
// Author: Ported from C++ OpenGL

const canvas = document.getElementById('glcanvas');
const gl = canvas.getContext('webgl2') || canvas.getContext('webgl');
if (!gl) {
  alert('WebGL not supported');
}

// Load shaders from script tags or fetch (for now, inline for demo)
let vertexShaderSource = '';
let fragmentShaderSource = '';

// Fetch shaders
Promise.all([
  fetch('shader.vert').then(r => r.text()),
  fetch('shader.frag').then(r => r.text())
]).then(([vs, fs]) => {
  vertexShaderSource = vs;
  fragmentShaderSource = fs;
  main();
});

function createShader(gl, type, source) {
  const shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    console.error('Shader compile failed:', gl.getShaderInfoLog(shader));
    gl.deleteShader(shader);
    return null;
  }
  return shader;
}

function createProgram(gl, vsSource, fsSource) {
  const vs = createShader(gl, gl.VERTEX_SHADER, vsSource);
  const fs = createShader(gl, gl.FRAGMENT_SHADER, fsSource);
  const program = gl.createProgram();
  gl.attachShader(program, vs);
  gl.attachShader(program, fs);
  gl.linkProgram(program);
  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    console.error('Program link failed:', gl.getProgramInfoLog(program));
    return null;
  }
  return program;
}

// Fibonacci sphere points generator (port from C++)
function generateFibonacciSpherePoints(samples) {
  const points = [];
  const phi = Math.PI * (3 - Math.sqrt(5));
  for (let i = 0; i < samples; i++) {
    const y = 1 - (i / (samples - 1)) * 2;
    const radius = Math.sqrt(1 - y * y);
    const theta = phi * i;
    const x = Math.cos(theta) * radius;
    const z = Math.sin(theta) * radius;
    points.push(x, y, z);
  }
  return new Float32Array(points);
}

// Spherical to Cartesian (port from C++)
function sphericalToCartesian(r, azimuthDeg, elevationDeg) {
  const az = azimuthDeg * Math.PI / 180;
  const el = elevationDeg * Math.PI / 180;
  const x = r * Math.cos(el) * Math.cos(az);
  const y = r * Math.sin(el);
  const z = r * Math.cos(el) * Math.sin(az);
  return [x, y, z];
}

function main() {
  const program = createProgram(gl, vertexShaderSource, fragmentShaderSource);
  gl.useProgram(program);

  // Sphere points
  const kNumPoints = 2048;
  const spherePoints = generateFibonacciSpherePoints(kNumPoints);

  // Buffer setup
  const vao = gl.createVertexArray();
  gl.bindVertexArray(vao);
  const vbo = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
  gl.bufferData(gl.ARRAY_BUFFER, spherePoints, gl.STATIC_DRAW);
  const posLoc = gl.getAttribLocation(program, 'aPos');
  gl.enableVertexAttribArray(posLoc);
  gl.vertexAttribPointer(posLoc, 3, gl.FLOAT, false, 0, 0);

  // Uniforms
  // MVP
  const uModel = gl.getUniformLocation(program, 'u_model');
  const uView = gl.getUniformLocation(program, 'u_view');
  const uProj = gl.getUniformLocation(program, 'u_projection');
  let model = glMatrix.mat4.create();
  let view = glMatrix.mat4.create();
  let projection = glMatrix.mat4.create();
  glMatrix.mat4.identity(model);
  glMatrix.mat4.identity(view);
  glMatrix.mat4.identity(projection);
  glMatrix.mat4.translate(view, view, [0, 0, -5]);
  glMatrix.mat4.perspective(projection, Math.PI/4, canvas.width/canvas.height, 0.1, 100.0);
  gl.uniformMatrix4fv(uModel, false, model);
  gl.uniformMatrix4fv(uView, false, view);
  gl.uniformMatrix4fv(uProj, false, projection);

  // Speaker positions and loudness
  const spkrPos = [
    ...sphericalToCartesian(1.0, 30.0, 0.0),
    ...sphericalToCartesian(1.0, -30.0, 0.0)
  ];
  const spkrDb = [1.0, 0.3];
  const uSpkrPos = gl.getUniformLocation(program, 'u_spkrPos');
  const uSpkrDb = gl.getUniformLocation(program, 'u_spkrDb');
  gl.uniform3fv(uSpkrPos, spkrPos);
  gl.uniform1fv(uSpkrDb, spkrDb);

  // Other uniforms (set as constants for now)
  gl.uniform1f(gl.getUniformLocation(program, 'u_waveSpeed'), 1.0);
  gl.uniform1f(gl.getUniformLocation(program, 'u_maxOverallDisplacement'), 0.3);
  gl.uniform1f(gl.getUniformLocation(program, 'u_spatialDecayRate'), 2.0);
  gl.uniform1f(gl.getUniformLocation(program, 'u_sourceDecayRate'), 1.5);
  gl.uniform1f(gl.getUniformLocation(program, 'u_oscillationFrequency'), 2.0);
  gl.uniform3f(gl.getUniformLocation(program, 'u_lightDir'), -1.0, 0.0, -1.0);
  gl.uniform3f(gl.getUniformLocation(program, 'u_lightColor'), 1.0, 1.0, 1.0);
  gl.uniform3f(gl.getUniformLocation(program, 'u_ambientColor'), 0.0, 1.0, 1.0);
  gl.uniform3f(gl.getUniformLocation(program, 'u_baseColor'), 0.153, 0.0936, 0.390);
  gl.uniform3f(gl.getUniformLocation(program, 'u_wavePeakColor'), 0.850, 0.0, 0.0);
  gl.uniform1f(gl.getUniformLocation(program, 'u_waveColorScale'), 5.0);

  // Animation loop
  function render(time) {
    time = time * 0.001;
    gl.uniform1f(gl.getUniformLocation(program, 'u_time'), time);
    gl.clearColor(0.2, 0.3, 0.3, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.bindVertexArray(vao);
    gl.enable(gl.PROGRAM_POINT_SIZE);
    gl.drawArrays(gl.POINTS, 0, kNumPoints);
    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}
