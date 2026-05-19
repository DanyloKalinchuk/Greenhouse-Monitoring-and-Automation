from django.apps import AppConfig

import sys
import os

REL_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(REL_PATH, "../../"))

from web import Web

class GreenhouseWebAppConfig(AppConfig):
    name = "greenhouse_web_app"

    def ready(self):
        Web.get_instance()