from django import template
register = template.Library()

import json

@register.filter
def jsonify(value):
    return json.dumps(value)
