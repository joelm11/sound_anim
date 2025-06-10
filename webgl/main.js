import { WaveShaderWeb } from './WaveShader.js';
import { SkyboxShaderWeb } from './SkyboxShader.js';
import { mat4, vec3 } from "./node_modules/gl-matrix/esm/index.js";

// Global variables
let g_azimuth = Math.PI / 4;     // 45 degrees initial horizontal angle
let g_elevation = Math.PI / 6;    // 30 degrees initial vertical angle
let g_radius = 8.0;              // Initial distance from origin
let g_lastX = 0;
let g_lastY = 0;
let g_dragging = false;

class Camera {
    constructor() {
        this.model = mat4.create();
        this.view = mat4.create();
        this.projection = mat4.create();
        this.position = vec3.create();
        this.lightPos = vec3.fromValues(0, 10, 0);
    }

    updateCamera(width, height) {
        // Calculate camera position from spherical coordinates
        const x = g_radius * Math.sin(g_elevation) * Math.sin(g_azimuth);
        const y = g_radius * Math.cos(g_elevation);
        const z = g_radius * Math.sin(g_elevation) * Math.cos(g_azimuth);
        
        vec3.set(this.position, x, y, z);
        mat4.lookAt(this.view, this.position, [0, 0, 0], [0, 1, 0]);
        mat4.perspective(this.projection, Math.PI / 4, width / height, 0.1, 100.0);
    }
}

async function main() {
    const canvas = document.querySelector('#glCanvas');
    const gl = canvas.getContext('webgl2');

    if (!gl) {
        alert('Unable to initialize WebGL. Your browser or machine may not support it.');
        return;
    }

    // Set up camera
    const camera = new Camera();

    // Initialize shaders
    const skyboxShader = new SkyboxShaderWeb(gl);
    await skyboxShader.init();
    const waveShader = new WaveShaderWeb(gl);
    await waveShader.init();

    // Load skybox texture
    const skyboxTexture = await loadSkyboxTexture(gl, 'resources/skybox.png');
    skyboxShader.setTexture(skyboxTexture);

    // Handle resize
    function resizeCanvas() {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        gl.viewport(0, 0, canvas.width, canvas.height);
        camera.updateCamera(canvas.width, canvas.height);
    }
    window.addEventListener('resize', resizeCanvas);
    resizeCanvas();

    // Mouse controls
    canvas.addEventListener('mousedown', (e) => {
        if (e.button === 0) { // Left mouse button
            g_dragging = true;
            g_lastX = e.clientX;
            g_lastY = e.clientY;
        }
    });

    canvas.addEventListener('mouseup', (e) => {
        if (e.button === 0) { // Left mouse button
            g_dragging = false;
        }
    });

    canvas.addEventListener('mousemove', (e) => {
        if (!g_dragging) return;
        
        const dx = e.clientX - g_lastX;
        const dy = e.clientY - g_lastY;
        g_lastX = e.clientX;
        g_lastY = e.clientY;

        const sensitivity = 0.01;
        g_azimuth -= dx * sensitivity;
        g_elevation -= dy * sensitivity;

        // Clamp elevation to avoid flipping (-89 to 89 degrees)
        const minElev = -Math.PI * 0.495;  // approximately -89 degrees
        const maxElev = Math.PI * 0.495;   // approximately 89 degrees
        g_elevation = Math.max(minElev, Math.min(maxElev, g_elevation));
        
        camera.updateCamera(canvas.width, canvas.height);
    });

    // Mouse wheel zoom
    canvas.addEventListener('wheel', (e) => {
        e.preventDefault();
        
        // Match C++ zoom behavior
        g_radius -= e.deltaY * 0.5 * 0.01; // Adjust for typical web wheel delta values
        
        // Clamp radius between min and max values (matching C++)
        g_radius = Math.max(0.3, Math.min(50.0, g_radius));
        
        camera.updateCamera(canvas.width, canvas.height);
    });

    // Render loop
    function render(time) {
        time *= 0.001;  // Convert to seconds

        gl.clearColor(0.2, 0.3, 0.3, 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        camera.updateCamera(canvas.width, canvas.height);

        // Draw skybox first (background)
        skyboxShader.draw(camera);

        // Draw waves
        waveShader.draw(time, camera, skyboxTexture);

        requestAnimationFrame(render);
    }
    requestAnimationFrame(render);
}

async function loadSkyboxTexture(gl, url) {
    const texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_CUBE_MAP, texture);

    const image = new Image();
    image.src = url;
    await image.decode();

    // Assuming the skybox image is in a cross format
    const faceWidth = image.width / 3;
    const faceHeight = image.height / 2;

    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    canvas.width = faceWidth;
    canvas.height = faceHeight;

    const faces = [
        { x: 2, y: 0 }, // positive x
        { x: 0, y: 0 }, // negative x
        { x: 1, y: 0 }, // positive z
        { x: 0, y: 1 }, // negative y
        { x: 1, y: 1 }, // positive y
        { x: 2, y: 1 }  // negative z
    ];

    faces.forEach((face, i) => {
        ctx.drawImage(image, 
            face.x * faceWidth, face.y * faceHeight, 
            faceWidth, faceHeight,
            0, 0, faceWidth, faceHeight);
        
        gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE,
            canvas);
    });

    gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_R, gl.CLAMP_TO_EDGE);

    return texture;
}

main();
