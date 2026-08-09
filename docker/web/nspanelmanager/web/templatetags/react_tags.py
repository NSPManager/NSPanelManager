import json

from django.template import Library
from django.utils.safestring import mark_safe

register = Library()


@register.simple_tag
def RC(component_name, **props):
    props_json = json.dumps(props)
    unique_id = f"react-{component_name}-{id(props)}"
    return mark_safe(f"<div id='{unique_id}' data-react-component='{component_name}' data-react-props='{props_json}'></div>")
