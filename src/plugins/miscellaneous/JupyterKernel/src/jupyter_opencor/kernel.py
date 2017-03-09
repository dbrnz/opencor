from jupyter_client.ioloop import IOLoopKernelManager

class OpenCORKernel(IOLoopKernelManager):
    def __init__(self, *args, **kwargs):
        super(OpenCORKernel, self).__init__(*args, **kwargs)
        self.log.info('NEW OpenCORKernel:  ARGS: %s  KWDS: %s', args, kwargs)
        self.autorestart = False
        self.connection_file = kwargs.get('connection_file')

    def _launch_kernel(self, *args, **kwargs):
        self.log.info('Launch OpenCORKernel:  ARGS: %s  KWDS: %s', args, kwargs)
        self.log.info("CONN: %s", self.parent.opencor_connection)
        self.parent.opencor_connection.send(self.connection_file)
