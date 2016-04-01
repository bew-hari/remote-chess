from flask_restful import Resource

class Test(Resource):
  def post(self):
    return {'status': 'success'}