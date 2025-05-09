from django import template
register = template.Library()

from web.models import Light, Switch

@register.filter
def is_instance(instance, check):
    return isinstance(instance, check)
