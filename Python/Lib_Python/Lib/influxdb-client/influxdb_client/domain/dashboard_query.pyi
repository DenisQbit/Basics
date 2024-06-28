from _typeshed import Incomplete

class DashboardQuery:
    openapi_types: Incomplete
    attribute_map: Incomplete
    discriminator: Incomplete
    def __init__(
        self,
        text: Incomplete | None = None,
        edit_mode: Incomplete | None = None,
        name: Incomplete | None = None,
        builder_config: Incomplete | None = None,
    ) -> None: ...
    @property
    def text(self): ...
    @text.setter
    def text(self, text) -> None: ...
    @property
    def edit_mode(self): ...
    @edit_mode.setter
    def edit_mode(self, edit_mode) -> None: ...
    @property
    def name(self): ...
    @name.setter
    def name(self, name) -> None: ...
    @property
    def builder_config(self): ...
    @builder_config.setter
    def builder_config(self, builder_config) -> None: ...
    def to_dict(self): ...
    def to_str(self): ...
    def __eq__(self, other): ...
    def __ne__(self, other): ...