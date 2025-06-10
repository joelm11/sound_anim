export class WaveShaderWeb {
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
        const vertexSource = await fetch('waves.vert').then(r => r.text());
        const fragmentSource = await fetch('waves.frag').then(r => r.text());

        this.program = this.createShaderProgram(vertexSource, fragmentSource);
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
        // Create plane mesh similar to your C++ implementation
        const mesh = this.generateSquarePlane(1024);

        this.vao = this.gl.createVertexArray();
        this.gl.bindVertexArray(this.vao);

        // Create and bind vertex buffer
        const vertexBuffer = this.gl.createBuffer();
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, vertexBuffer);
        this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(mesh.vertices), this.gl.STATIC_DRAW);

        // Enable vertex attributes
        this.gl.enableVertexAttribArray(0);
        this.gl.vertexAttribPointer(0, 3, this.gl.FLOAT, false, 6 * Float32Array.BYTES_PER_ELEMENT, 0);
        this.gl.enableVertexAttribArray(1);
        this.gl.vertexAttribPointer(1, 3, this.gl.FLOAT, false, 6 * Float32Array.BYTES_PER_ELEMENT, 3 * Float32Array.BYTES_PER_ELEMENT);

        // Create and bind index buffer
        const indexBuffer = this.gl.createBuffer();
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
        this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, new Uint32Array(mesh.indices), this.gl.STATIC_DRAW);

        this.indexCount = mesh.indices.length;
    }

    generateSquarePlane(resolution) {
        const vertices = [];
        const indices = [];
        const size = 16.0;
        const step = size / resolution;

        for (let z = 0; z <= resolution; z++) {
            for (let x = 0; x <= resolution; x++) {
                const xPos = (x * step) - (size / 2);
                const zPos = (z * step) - (size / 2);
                
                // Position
                vertices.push(xPos, 0, zPos);
                // Normal (initially pointing up)
                vertices.push(0, 1, 0);

                if (z < resolution && x < resolution) {
                    const topLeft = z * (resolution + 1) + x;
                    const topRight = topLeft + 1;
                    const bottomLeft = (z + 1) * (resolution + 1) + x;
                    const bottomRight = bottomLeft + 1;

                    indices.push(topLeft, bottomLeft, topRight);
                    indices.push(bottomLeft, bottomRight, topRight);
                }
            }
        }

        return { vertices, indices };
    }

    initUniforms() {
        this.gl.useProgram(this.program);

        // Get uniform locations
        this.uniforms = {
            model: this.gl.getUniformLocation(this.program, 'u_model'),
            view: this.gl.getUniformLocation(this.program, 'u_view'),
            projection: this.gl.getUniformLocation(this.program, 'u_projection'),
            time: this.gl.getUniformLocation(this.program, 'u_time'),
            lightPos: this.gl.getUniformLocation(this.program, 'u_lightpos'),
            cameraPos: this.gl.getUniformLocation(this.program, 'u_camerapos'),
            skyboxTexture: this.gl.getUniformLocation(this.program, 'u_skyboxTexture'),
            mieCoefficient: this.gl.getUniformLocation(this.program, 'u_mieCoefficient'),
            horizonBlurStrength: this.gl.getUniformLocation(this.program, 'u_horizonBlurStrength'),
            horizonHeight: this.gl.getUniformLocation(this.program, 'u_horizonHeight')
        };

        // Wave parameters
        this.uniforms.amplitudes = [];
        this.uniforms.frequencies = [];
        this.uniforms.phases = [];
        this.uniforms.waveDirs = [];

        for (let i = 0; i < 32; i++) {
            this.uniforms.amplitudes.push(this.gl.getUniformLocation(this.program, `u_amplitudes[${i}]`));
            this.uniforms.frequencies.push(this.gl.getUniformLocation(this.program, `u_frequencies[${i}]`));
            this.uniforms.phases.push(this.gl.getUniformLocation(this.program, `u_phases[${i}]`));
            this.uniforms.waveDirs.push(this.gl.getUniformLocation(this.program, `u_wavedirs[${i}]`));
        }

        // Set default values
        this.gl.uniform1f(this.uniforms.mieCoefficient, 0.02);
        this.gl.uniform1f(this.uniforms.horizonBlurStrength, 5.0);
        this.gl.uniform1f(this.uniforms.horizonHeight, 0.0);

        // Set some initial wave parameters
        for (let i = 0; i < 32; i++) {
            this.gl.uniform1f(this.uniforms.amplitudes[i], 0.5 / (i + 1));
            this.gl.uniform1f(this.uniforms.frequencies[i], Math.random());
            this.gl.uniform1f(this.uniforms.phases[i], i * Math.PI / 4.0);
            const angle = Math.random() * Math.PI / 2.0;
            this.gl.uniform2f(this.uniforms.waveDirs[i], Math.cos(angle), Math.sin(angle));
        }
        // // Set remaining waves to 0
        // for (let i = 4; i < 32; i++) {
        //     this.gl.uniform1f(this.uniforms.amplitudes[i], 0.0);
        //     this.gl.uniform1f(this.uniforms.frequencies[i], 0.0);
        //     this.gl.uniform1f(this.uniforms.phases[i], 0.0);
        //     this.gl.uniform2f(this.uniforms.waveDirs[i], 0.0, 0.0);
        // }
    }

    use() {
        this.gl.useProgram(this.program);
    }

    draw(time, camera, skyboxTexture) {
        this.use();

        // Update uniforms
        this.gl.uniformMatrix4fv(this.uniforms.model, false, camera.model);
        this.gl.uniformMatrix4fv(this.uniforms.view, false, camera.view);
        this.gl.uniformMatrix4fv(this.uniforms.projection, false, camera.projection);
        this.gl.uniform1f(this.uniforms.time, time);
        
        this.gl.uniform3fv(this.uniforms.lightPos, camera.lightPos);
        this.gl.uniform3fv(this.uniforms.cameraPos, camera.position);

        // Bind skybox texture
        this.gl.activeTexture(this.gl.TEXTURE1);
        this.gl.bindTexture(this.gl.TEXTURE_CUBE_MAP, skyboxTexture);
        this.gl.uniform1i(this.uniforms.skyboxTexture, 1);

        // Enable depth test
        this.gl.enable(this.gl.DEPTH_TEST);
        this.gl.depthFunc(this.gl.LESS);

        // Draw the mesh
        this.gl.bindVertexArray(this.vao);
        this.gl.drawElements(this.gl.TRIANGLES, this.indexCount, this.gl.UNSIGNED_INT, 0);
        this.gl.bindVertexArray(null);
    }
}
