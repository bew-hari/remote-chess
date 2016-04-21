from flask_restful import Resource, reqparse

parser = reqparse.RequestParser()
parser.add_argument('board_id')

class Register(Resource):
  # register board
  def post(self):
    args = parser.parse_args()
    return {
      'error': None,
      'board_id': args['board_id']
    }, 201

class Authenticate(Resource):
  # authenticate board with id
  def post(self):
    return {'error': 'None'}
