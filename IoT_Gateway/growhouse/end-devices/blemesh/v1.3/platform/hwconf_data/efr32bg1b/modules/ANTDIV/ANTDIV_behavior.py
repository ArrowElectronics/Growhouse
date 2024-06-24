from . import ExporterModel
from . import ANTDIV_model
from . import RuntimeModel


class ANTDIV(ExporterModel.Module):
    def __init__(self, name=None):
        if not name:
            name = self.__class__.__name__
        super(ANTDIV, self).__init__(name, visible=True)
        self.model = ANTDIV_model
