from rest_framework import exceptions
from rest_framework.authentication import BaseAuthentication, CSRFCheck


class CsrfOnlyAuthentication(BaseAuthentication):
    """Authenticates nobody, but rejects unsafe requests that fail Django's CSRF check.

    DRF marks its views csrf_exempt and leaves the CSRF check to SessionAuthentication, which only
    runs it for logged-in users. NSPanelManager has no user accounts, so without this the /rest/v2/
    views would accept cross-site requests that CsrfViewMiddleware rejects for the plain views.
    This is the same check SessionAuthentication.enforce_csrf() makes.
    """

    def authenticate(self, request):
        check = CSRFCheck(lambda _request: None)
        check.process_request(request)
        reason = check.process_view(request, None, (), {})
        if reason:
            raise exceptions.PermissionDenied(f"CSRF Failed: {reason}")
        return None
