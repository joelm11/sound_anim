import { mat4 } from "./node_modules/gl-matrix/esm/index.js";

export class SkyboxShaderWeb {
    constructor(gl) {
        this.gl = gl;
        // Initialize in an async context to ensure shaders are loaded
        this.init();
    }

    async init() {
        await this.initShaders();
        this.initBuffers();
        this.initUniforms();
    }

    async initShaders() {
        const vertexSource = await fetch('skybox.vert').then(r => r.text());
        const fragmentSource = await fetch('skybox.frag').then(r => r.text());

        this.program = this.createShaderProgram(vertexSource, fragmentSource);
    }
    
    setTexture(textureId) {
        this.textureId = textureId;
    }

    createShaderProgram(vertexSource, fragmentSource) {
        const vertexShader = this.compileShader(this.gl.VERTEX_SHADER, vertexSource);
        const fragmentShader = this.compileShader(this.gl.FRAGMENT_SHADER, fragmentSource);

        const program = this.gl.createProgram();
        this.gl.attachShader(program, vertexShader);
        this.gl.attachShader(program, fragmentShader);
        this.gl.linkProgram(program);

        if (!this.gl.getProgramParameter(program, this.gl.LINK_STATUS)) {
            console.error('Failed to initialize shader program:', this.gl.getProgramInfoLog(program));
            return null;
        }

        return program;
    }

    compileShader(type, source) {
        const shader = this.gl.createShader(type);
        this.gl.shaderSource(shader, source);
        this.gl.compileShader(shader);

        if (!this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS)) {
            console.error('Failed to compile shader:', this.gl.getShaderInfoLog(shader));
            this.gl.deleteShader(shader);
            return null;
        }

        return shader;
    }

    initBuffers() {
        // Create cube vertices for skybox
        const vertices = new Float32Array([
            -1.0,  1.0, -1.0,
            -1.0, -1.0, -1.0,
             1.0, -1.0, -1.0,
             1.0,  1.0, -1.0,
            -1.0, -1.0,  1.0,
            -1.0,  1.0,  1.0,
             1.0, -1.0,  1.0,
             1.0,  1.0,  1.0,
        ]);

        const indices = new Uint16Array([
            0, 1, 2, 0, 2, 3,  // front
            5, 4, 6, 5, 6, 7,  // back
            5, 0, 3, 5, 3, 7,  // top
            1, 4, 6, 1, 6, 2,  // bottom
            5, 4, 1, 5, 1, 0,  // left
            3, 2, 6, 3, 6, 7   // right
        ]);

        this.vao = this.gl.createVertexArray();
        this.gl.bindVertexArray(this.vao);

        const vertexBuffer = this.gl.createBuffer();
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, vertexBuffer);
        this.gl.bufferData(this.gl.ARRAY_BUFFER, vertices, this.gl.STATIC_DRAW);

        const indexBuffer = this.gl.createBuffer();
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
        this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, indices, this.gl.STATIC_DRAW);

        this.gl.enableVertexAttribArray(0);
        this.gl.vertexAttribPointer(0, 3, this.gl.FLOAT, false, 0, 0);

        this.indexCount = indices.length;
    }

    initUniforms() {
        this.gl.useProgram(this.program);
        this.uniforms = {
            view: this.gl.getUniformLocation(this.program, 'u_view'),
            projection: this.gl.getUniformLocation(this.program, 'u_projection')
        };
    }

    draw(camera) {
        this.gl.useProgram(this.program);
        
        // Remove translation from view matrix for skybox
        const viewMatrix = mat4.clone(camera.view);
        viewMatrix[12] = 0;
        viewMatrix[13] = 0;
        viewMatrix[14] = 0;

        this.gl.uniformMatrix4fv(this.uniforms.view, false, viewMatrix);
        this.gl.uniformMatrix4fv(this.uniforms.projection, false, camera.projection);

        // Draw skybox first
        this.gl.depthFunc(this.gl.LEQUAL);
        this.gl.bindVertexArray(this.vao);
        this.gl.drawElements(this.gl.TRIANGLES, this.indexCount, this.gl.UNSIGNED_SHORT, 0);
        this.gl.bindVertexArray(null);
        this.gl.depthFunc(this.gl.LESS);
    }
}
