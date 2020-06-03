from . import ExporterModel
from . import IOEXP_model
from . import RuntimeModel


class IOEXP(ExporterModel.Module):
    def __init__(self, name=None):
        if not name:
            name = self.__class__.__name__
        super(IOEXP, self).__init__(name, visible=True)
        self.model = IOEXP_model

    def set_runtime_hooks(self):
        pass
