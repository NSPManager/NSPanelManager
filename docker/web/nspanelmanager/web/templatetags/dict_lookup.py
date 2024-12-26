from django import template
register = template.Library()

import json

@register.filter
def dict_lookup(dict, key):
    return dict.get(key)
