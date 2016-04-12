from flask import Flask
from flask_restful import Api

from resources.auth import *
from resources.chess import *

app = Flask(__name__)
api = Api(app)

# auth resources
api.add_resource(Register, '/register')
api.add_resource(Authenticate, '/auth')

# game resources
api.add_resource(Games, '/games', methods=['POST'])
api.add_resource(Games, '/games/<game_id>', methods=['GET', 'POST'])

if __name__ == '__main__':
  app.run(debug=True)
