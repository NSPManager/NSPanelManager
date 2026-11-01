"""
drf-spectacular is installed for development and CI only (docker/web/requirements-dev.txt): it depends
on rpds-py, which has no wheel for every architecture the release image is built for. Views import its
decorators from here, and in the release image they do nothing.
"""

try:
    from drf_spectacular.utils import extend_schema
except ImportError:

    def extend_schema(*args, **kwargs):
        def decorator(view):
            return view

        return decorator


__all__ = ["extend_schema"]
