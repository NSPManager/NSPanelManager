from django import template
register = template.Library()

@register.filter
def is_instance(instance, check):
    return isinstance(instance, check)
