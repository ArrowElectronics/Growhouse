from . import ExporterModel
from . import USART_model
from . import RuntimeModel


class USART(ExporterModel.Module):
    def __init__(self, name=None):
        if not name:
            name = self.__class__.__name__
        super(USART, self).__init__(name, visible=True, core=True)
        self.model = USART_model

    def set_runtime_hooks(self):
        pass
