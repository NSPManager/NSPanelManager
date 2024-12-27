from django import template
register = template.Library()

import json

@register.filter
def jsonify(value):
    return json.dumps(value)

@register.filter
def jsonify_str(value):
    return "'" + str(json.dumps(value)) + "'"
