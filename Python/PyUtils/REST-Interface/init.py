__author__ = 'Josh Allen Bosley'

_hello_resp = '''
<html>
    <head>
        <title> Hola {name} </title>
    </head>
    <body>
        <h1> Hello {name} ! </h1>
    </body>
</html>
'''

def hello_world(environ, start_response):
    start_response('200 OK', [('Content-type', 'text/html')])
    params = environ['params']
    resp = _hello_resp.format(name=params.get('name'))
    yield resp.encode('utf-8')

if __name__ == '__main__':
    from REST import PathDispatcher
    from wsgiref.simple_server import make_server

    # Create dispatcher and register functions
    dispatcher = PathDispatcher()
    dispatcher.register('GET', '/hello', hello_world)

    # Launch the basic http server
    httpd = make_server('', 8080, dispatcher)
    print('Serving on port 8080')
    httpd.serve_forever()