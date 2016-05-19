#from flask import Flask
from remote_chess import app
from flask_restful import Api
#from flask_pymongo import PyMongo

from remote_chess.auth import *
from remote_chess.game import *

#app = Flask(__name__)
#app.config['MONGO_DBNAME'] = 'remote_chess'

#mongo = PyMongo(app, config_prefix='MONGO')

api = Api(app)

# auth resources
api.add_resource(Register, '/register')
api.add_resource(Authenticate, '/auth')

# game resources
api.add_resource(Games, '/create', methods=['POST'])
api.add_resource(Game, '/games', methods=['GET', 'POST'])

if __name__ == '__main__':
  app.run(debug=True)
