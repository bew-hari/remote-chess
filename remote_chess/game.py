from flask import jsonify
from flask_restful import Resource, reqparse
from flask_pymongo import PyMongo

from bson.objectid import ObjectId
from bson.json_util import dumps, loads

import requests
import chess
import chess.uci

from remote_chess.db import mongo

class Games(Resource):
  # create new game
  def post(self):
    parser = reqparse.RequestParser()
    parser.add_argument('board_id')
    parser.add_argument('type', type=int) # 0 = AI, 1 = human

    args = parser.parse_args()

    if args['type'] == 0:
      # create game with AI
      board = chess.Board()
      game = {
        '_id': str(ObjectId()),
        'board': board.fen(),
        'status': 2,
        'result': '',
        'players': [args['board_id']],
        'type': args['type']
      }

      # save game
      id = mongo.db.games.save(game)

      # TODO: post back to white for move

      return {
        'error': None,
        'data': game
      }, 201

    else:
      game = mongo.db.games.find_one({
        'type': 1,
        'status': 1,
        'players': {'$size': 1}
      })

      if game:
        # join game
        game['status'] = 2
        players = game['players']
        players.append(args['board_id'])

        # update game in database
        mongo.db.games.update(
          {'_id': game['_id']}, 
          {
            '$set': {
              'players': players,
              'status': game['status']
            },
            '$currentDate': {'lastModified': True}
          }
        )

        # TODO: post to white for move
        
        # TODO: post to black to wait
        
      else:
        # create game
        board = chess.Board()
        game = {
          '_id': str(ObjectId()),
          'board': board.fen(),
          'status': 1,
          'result': None,
          'players': [args['board_id']],
          'type': args['type']
        }
        
        # save game
        id = mongo.db.games.save(game)

    return {
      'error': None,
      'data': game
    }, 201




class Game(Resource):
  # get specific game
  def get(self):
    parser = reqparse.RequestParser()
    parser.add_argument('game_id')
    args = parser.parse_args()

    game = mongo.db.games.find_one({'_id': args['game_id']})
    
    if game:
      res = {
        'error': None,
        'data': {
          '_id': game['_id'],
          'board': game['board'],
          'status': game['status'],
          'result': game['result'], 
          'players': game['players'],
          'type': game['type']
        }
      }
    else:
      res = {
        'error': 'No game with matching _id',
        'data': None
      }

    return res, 201
  
  # modify specific game
  def post(self):
    parser = reqparse.RequestParser()
    parser.add_argument('board_id')
    parser.add_argument('game_id')
    parser.add_argument('move')

    args = parser.parse_args()

    game = mongo.db.games.find_one({
      '_id': args['game_id'],
      'players': args['board_id'],
      'status': 2
    })

    if not game:
      return {
        'error': 'No game with matching id',
        'data': None
      }, 201

    player = args['board_id']
    board = chess.Board(fen=game['board'])
    move = chess.Move.from_uci(args['move'])

    if board.turn != (player == game['players'][0]):
      return {
        'error': 'Other player\'s turn',
        'data': None
      }, 201

    if move not in board.legal_moves:
      return {
        'error': 'Illegal move',
        'data': None
      }, 201

    board.push(move)
    to_move = ''
    result = ''
    text = ''

    if game['type'] == 0:
      if board.is_game_over():
        game['status'] = 3
        result = board.result()
        text = 'You win' if result == '1-0' else 'Draw'
      else:
        # generate move for AI
        engine = chess.uci.popen_engine('./remote_chess/stockfish-7-x64-linux')
        engine.position(board)
        move = engine.go(movetime=2000)

        board.push(move.bestmove)

        if board.is_game_over():
          game['status'] = 3
          result = board.result()
          text = 'You lose' if result == '0-1' else 'Draw'

        to_move = move.bestmove.uci()

    else:
      if board.is_game_over():
        game['status'] = 3
        result = board.result()
        if (player == game['players'][0] and result == '1-0') \
            or (player == game['players'][1] and result == '0-1'):
          text = 'You win'
        else:
          text = 'Draw'

      opponent = game['players'][(player == game['players'][0])]
      # TODO: post player move to other board. use move.uci()

    # update game in database
    mongo.db.games.update(
      {'_id': args['game_id']}, 
      {
        '$set': {
          'board': board.fen(),
          'status': game['status'],
          'result': result
        },
        '$currentDate': {'lastModified': True}
      }
    )

    return {
      'error': None,
      'data': {
        '_id': game['_id'],
        'board': board.fen(),
        'status': game['status'],
        'turn': 0,
        'result': result,
        'result_text': text,
        'to_move': to_move,
        'players': game['players'],
        'type': game['type']
      }
    }, 201
