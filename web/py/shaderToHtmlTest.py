import unittest
import textwrap
import os
from shaderToHtml import convert_shader_to_html


class ShaderToHtmlTest(unittest.TestCase):

    def setUp(self):
        self.shader_source = \
            '''
            #version 410 core
            in vec3 vertexWorld;
            out vec3 vertexCamera;

            void main() {
                vertexCamera = (modelViewTransform * vec4(vertexWorld, 1.0)).xyz;
                vertexColor = lineColor;
                gl_Position = projectionTransform * modelViewTransform * vec4(vertexWorld, 1.0);
            }
            '''

    def tearDown(self):
        os.remove(self.shader_file_name)
        os.remove('shader.html')

    def writeShaderFile(self):
        with open(self.shader_file_name, 'w') as shader_file:
            shader_file.write(textwrap.dedent(self.shader_source))

    def test_convertVertexShaderToHtml(self):
        self.shader_file_name = 'vertexShader.glsl'
        self.writeShaderFile()
        convert_shader_to_html(self.shader_file_name, 'shader.html')
        with open('shader.html', 'r') as html_file:
            html_contents = html_file.read()
            # assert shader source is reproduced
            self.assertNotEqual(html_contents.find(textwrap.dedent(self.shader_source.replace('410 core', '300 es'))), -1)
            # assert shader type is vertex since the file name includes the word "vertex"
            self.assertNotEqual(html_contents.find('type="x-shader/x-vertex"'), -1)
            # assert that script tag's id is the vertex shader's file name
            self.assertNotEqual(html_contents.find('script id="vertexShader"'), -1)

    def test_convertFragmentShaderToHtml(self):
        self.shader_file_name = 'fragmentShader.glsl'
        self.writeShaderFile()
        convert_shader_to_html(self.shader_file_name, 'shader.html')
        with open('shader.html', 'r') as html_file:
            html_contents = html_file.read()
            # assert shader source is reproduced
            self.assertNotEqual(html_contents.find(textwrap.dedent(self.shader_source.replace('410 core', '300 es'))), -1)
            # assert shader type is fragment since the file name includes the word "fragment"
            self.assertNotEqual(html_contents.find('type="x-shader/x-fragment"'), -1)
            # assert that script tag's id is the fragment shader's file name
            self.assertNotEqual(html_contents.find('script id="fragmentShader"'), -1)


if __name__ == '__main__':
    unittest.main()
