from __future__ import absolute_import

import logging

import traceback
import uuid

from tornado import gen, httpserver
from tornado.concurrent import Future
from tornado.ioloop import IOLoop

from ipython_genutils.py3compat import unicode_type

from notebook.notebookapp import (
    NotebookApp,
    NotebookWebApplication,
    load_handlers,
    random_ports,
    )

from notebook.base.handlers import (
    FileFindHandler,
    Template404,
    RedirectWithParams,
    )

from notebook.services.sessions.sessionmanager import SessionManager
from notebook.services.kernels.kernelmanager import MappingKernelManager
from notebook.utils import url_path_join

from traitlets.config.application import catch_config_error


class OpenCORSessionManager(SessionManager):

    def __init__(self, *args, **kwargs):
        super(OpenCORSessionManager, self).__init__(*args, **kwargs)
        self.kernel_ids = { }

    def create_session(self, kernel_name=None, **kwargs):
        """Creates a session and returns its model"""
        session_id = self.new_session_id()
        self.log.info("New session: %s" % session_id)

        kernel_id = self.kernel_ids.get(kernel_name)
        if kernel_id is None:
            km = self.kernel_manager
            kernel_id = km.start_kernel(kernel_name=kernel_name)
            self.kernel_ids[kernel_name] = kernel_id

        return self.save_session(session_id, kernel_id=kernel_id)


class OpenCORKernelManager(MappingKernelManager):

    def __init__(self, *args, **kwargs):
        self.log.info('OpenCORKM:  ARGS: %s  KWDS: %s', args, kwargs)
        # 'kernel_name': 'opencor'
#        from pudb import set_trace; set_trace()
        self.log.info("KM Class: %s", self.kernel_manager_class)
        super(OpenCORKernelManager, self).__init__(*args, **kwargs)

    def start_kernel(self, kernel_id=None, path=None, **kwargs):
        if kernel_id is None:
            if path is not None:
                kwargs['cwd'] = self.cwd_for_path(path)
            kernel_id = unicode_type(uuid.uuid4())
            self.log.info("New kernel: %s" % kernel_id)

            super(MappingKernelManager, self).start_kernel(kernel_id=kernel_id, **kwargs)

            self.log.info("Kernel started: %s" % kernel_id)
            self.log.debug("Kernel args: %r" % kwargs)
            # register callback for failed auto-restart
            self.add_restart_callback(kernel_id,
                lambda : self._handle_kernel_died(kernel_id),
                'dead',
            )
        else:
            self._check_kernel_id(kernel_id)
            self.log.info("Using existing kernel: %s" % kernel_id)

        return kernel_id

    def shutdown_kernel(self, kernel_id, now=False):
        """Shutdown a kernel by kernel_id"""
        self._check_kernel_id(kernel_id)
        return super(MappingKernelManager, self).shutdown_kernel(kernel_id, now=now)



class OpenCORServerWebApplication(NotebookWebApplication):

    def init_handlers(self, settings):
        """Load the (URL pattern, handler) tuples for each component."""

        logging.info("Loading handlers...")

        # Order matters. The first handler to match the URL will handle the request.
        handlers = []
        handlers.extend(load_handlers('tree.handlers'))
#        handlers.extend([(r"/login", settings['login_handler_class'])])
#        handlers.extend([(r"/logout", settings['logout_handler_class'])])
        handlers.extend(load_handlers('files.handlers'))
        handlers.extend(load_handlers('notebook.handlers'))
        handlers.extend(load_handlers('nbconvert.handlers'))
        handlers.extend(load_handlers('kernelspecs.handlers'))
        handlers.extend(load_handlers('edit.handlers'))
        handlers.extend(load_handlers('services.api.handlers'))
        handlers.extend(load_handlers('services.config.handlers'))
        handlers.extend(load_handlers('services.kernels.handlers')) ##
        handlers.extend(load_handlers('services.contents.handlers'))
        handlers.extend(load_handlers('services.sessions.handlers'))
        handlers.extend(load_handlers('services.nbconvert.handlers'))
        handlers.extend(load_handlers('services.kernelspecs.handlers'))
#        handlers.extend(load_handlers('services.security.handlers'))

        # BEGIN HARDCODED WIDGETS HACK
        # TODO: Remove on notebook 5.0
        try:
            import widgetsnbextension
        except:
            try:
                import ipywidgets as widgets
                handlers.append(
                    (r"/nbextensions/widgets/(.*)", FileFindHandler, {
                        'path': widgets.find_static_assets(),
                        'no_cache_paths': ['/'], # don't cache anything in nbextensions
                    }),
                )
            except:
                app_log.warning('Widgets are unavailable. Please install widgetsnbextension or ipywidgets 4.0')
        # END HARDCODED WIDGETS HACK

        handlers.append(
            (r"/nbextensions/(.*)", FileFindHandler, {
                'path': settings['nbextensions_path'],
                'no_cache_paths': ['/'], # don't cache anything in nbextensions
            }),
        )
        handlers.append(
            (r"/custom/(.*)", FileFindHandler, {
                'path': settings['static_custom_path'],
                'no_cache_paths': ['/'], # don't cache anything in custom
            })
        )
        # register base handlers last
        handlers.extend(load_handlers('base.handlers'))
        # set the URL that will be redirected from `/`
        handlers.append(
            (r'/?', RedirectWithParams, {
                'url' : settings['default_url'],
                'permanent': False, # want 302, not 301
            })
        )

        # prepend base_url onto the patterns that we match
        new_handlers = []
        for handler in handlers:
            pattern = url_path_join(settings['base_url'], handler[0])
            new_handler = tuple([pattern] + list(handler[1:]))
            new_handlers.append(new_handler)
        # add 404 on the end, which will catch everything that falls through
        new_handlers.append((r'(.*)', Template404))
        return new_handlers


class OpenCORServerApp(NotebookApp):

    def init_webapp(self):
        """initialize tornado webapp and httpserver"""

        logging.info("Initing web app...")

        self.tornado_settings['allow_origin'] = self.allow_origin
        if self.allow_origin_pat:
            self.tornado_settings['allow_origin_pat'] = re.compile(self.allow_origin_pat)
        self.tornado_settings['allow_credentials'] = self.allow_credentials
        self.tornado_settings['cookie_options'] = self.cookie_options
        self.tornado_settings['token'] = self.token
        if (self.open_browser or self.file_to_run) and not self.password:
            self.one_time_token = binascii.hexlify(os.urandom(24)).decode('ascii')
            self.tornado_settings['one_time_token'] = self.one_time_token
        # ensure default_url starts with base_url
        if not self.default_url.startswith(self.base_url):
            self.default_url = url_path_join(self.base_url, self.default_url)

        self.web_app = OpenCORServerWebApplication(
            self, self.kernel_manager, self.contents_manager,
            self.session_manager, self.kernel_spec_manager,
            self.config_manager,
            self.log, self.base_url, self.default_url, self.tornado_settings,
            self.jinja_environment_options
        )
        ssl_options = self.ssl_options
        if self.certfile:
            ssl_options['certfile'] = self.certfile
        if self.keyfile:
            ssl_options['keyfile'] = self.keyfile
        if self.client_ca:
            ssl_options['ca_certs'] = self.client_ca
        if not ssl_options:
            # None indicates no SSL config
            ssl_options = None
        else:
            # SSL may be missing, so only import it if it's to be used
            import ssl
            # Disable SSLv3 by default, since its use is discouraged.
            ssl_options.setdefault('ssl_version', ssl.PROTOCOL_TLSv1)
            if ssl_options.get('ca_certs', False):
                ssl_options.setdefault('cert_reqs', ssl.CERT_REQUIRED)

        self.login_handler_class.validate_security(self, ssl_options=ssl_options)
        self.http_server = httpserver.HTTPServer(self.web_app, ssl_options=ssl_options,
                                                 xheaders=self.trust_xheaders)

        success = None
        for port in random_ports(self.port, self.port_retries+1):
            try:
                self.http_server.listen(port, self.ip)
            except socket.error as e:
                if e.errno == errno.EADDRINUSE:
                    self.log.info('The port %i is already in use, trying another port.' % port)
                    continue
                elif e.errno in (errno.EACCES, getattr(errno, 'WSAEACCES', errno.EACCES)):
                    self.log.warn("Permission to listen on port %i denied" % port)
                    continue
                else:
                    raise
            else:
                self.port = port
                success = True
                break
        if not success:
            self.log.critical('ERROR: the notebook server could not be started because '
                              'no available port could be found.')
            self.exit(1)

    @catch_config_error
    def initialize(self, argv=None):
        super(OpenCORServerApp, self).initialize(argv)

        # Create a kernel which won't be launched as it is already running...

        kernel_id = self.session_manager.create_session(kernel_name='opencor')


def main(opencor_connection):
#    from pudb import set_trace; set_trace()

    OpenCORServerApp.open_browser = False
    OpenCORServerApp.token = 'xxxyy'
    OpenCORServerApp.session_manager_class = OpenCORSessionManager

    OpenCORKernelManager.kernel_manager_class = 'jupyter_opencor.kernel.OpenCORKernel'
    OpenCORKernelManager.opencor_connection = opencor_connection

    OpenCORServerApp.kernel_manager_class = OpenCORKernelManager
##    OpenCORServerApp.tornado_settings = { 'autoreload': True }

    OpenCORServerApp.launch_instance()


if __name__ == '__main__':
    main()
