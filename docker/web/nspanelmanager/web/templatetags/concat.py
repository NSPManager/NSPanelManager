from django import template
register = template.Library()

import json

@register.filter
def concat(arg1, arg2):
    return str(arg1) + str(arg2)

@register.simple_tag
def concat_all(*args):
    return ''.join(map(str, args))
