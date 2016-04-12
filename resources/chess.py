from flask_restful import Resource

class Games(Resource):
	# create new game
  def post(self):
    return {'status': 'success'}

class Game(Resource):
	# get specific game
	def get(self, game_id):
		return {'status': 'GET in Game resource'}
	
	# modify specific game
	def post(self):
		return {'status': 'POST in Game resource'}