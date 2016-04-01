from flask import Flask
from flask_restful import Api

from resources.auth import *

app = Flask(__name__)
api = Api(app)

api.add_resource(Test, '/test')

if __name__ == '__main__':
  app.run(debug=True)
