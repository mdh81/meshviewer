import sys
import os
import textwrap


def convert_shader_to_html(shader_file_name, html_file_name):

    with open(shader_file_name, 'r') as shader_file, open(html_file_name, 'a') as html_file:

        html_content = \
            '''
            <script id="{}" type="x-shader/{}">
            {}
            </script>
            '''

        shader_file_name = os.path.basename(shader_file_name).split('.')[0]
        is_vertex_shader = shader_file_name.find('vertex') != -1 or shader_file_name.find('Vertex') != -1
        glsl_content = shader_file.read()
        html_content = textwrap.dedent(html_content).\
            format(shader_file_name,
                   'x-{}'.format('vertex' if is_vertex_shader else 'fragment'),
                   glsl_content.replace('410 core', '300 es'))
        html_file.write(html_content)


def print_usage_and_exit():
    print('Usage:', sys.argv[0], '<shader source file> <html output file>')
    sys.exit(-1)


def parse_arguments():
    if len(sys.argv) != 3:
        print_usage_and_exit()
    if not os.path.exists(sys.argv[1]):
        print(sys.argv[1], 'does not exist')
        print_usage_and_exit()
    return sys.argv[1], sys.argv[2]


if __name__ == '__main__':
    convert_shader_to_html(*parse_arguments())