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
        'status': 1,
        'result': '',
        'players': [args['board_id']],
        'type': args['type']
      }

      # save game
      id = mongo.db.games.save(game)

      game['turn'] = True

    else:
      game = mongo.db.games.find_one({
        'type': 1,
        'status': 0,
        'players': {'$size': 1}
      })

      if game:
        # join game
        game['status'] = 1
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

        game['turn'] = True
        # TODO: post to first board for move
        
        game['turn'] = False
        
      else:
        # create game
        board = chess.Board()
        game = {
          '_id': str(ObjectId()),
          'board': board.fen(),
          'status': 0,
          'result': None,
          'players': [args['board_id']],
          'type': args['type']
        }
        
        # save game
        id = mongo.db.games.save(game)

        game['turn'] = False

    game['to_move'] = None
    game['result_text'] = None
    return {
      'error': None,
      'data': game
    }, 201




class Game(Resource):
  # get specific game
  def get(self, game_id):
    game = mongo.db.games.find_one({'_id': game_id})
    
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
  def post(self, game_id):
    parser = reqparse.RequestParser()
    parser.add_argument('board_id')
    parser.add_argument('move')

    args = parser.parse_args()

    game = mongo.db.games.find_one({
      '_id': game_id,
      'players': args['board_id'],
      'status': 1
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
    turn = False
    to_move = None
    result = None
    text = None

    if game['type'] == 0:
      turn = True

      if board.is_game_over():
        game['status'] = 2
        result = board.result()
        text = 'You win' if result == '1-0' else 'Draw'
      else:
        # generate move for AI
        engine = chess.uci.popen_engine('./remote_chess/stockfish-7-x64-linux')
        engine.position(board)
        move = engine.go(movetime=2000)

        board.push(move.bestmove)

        if board.is_game_over():
          game['status'] = 2
          result = board.result()
          text = 'You lose' if result == '0-1' else 'Draw'

        to_move = move.bestmove.uci()

    else:
      if board.is_game_over():
        game['status'] = 2
        result = board.result()
        if (player == game['players'][0] and result == '1-0') \
            or (player == game['players'][1] and result == '0-1'):
          text = 'You win'
        else:
          text = 'Draw'

      turn = True
      to_move = move.uci()
      opponent = game['players'][(player == game['players'][0])]
      # TODO: post player move to other board

      # reset values for this board
      turn = False
      to_move = None

    # update game in database
    mongo.db.games.update(
      {'_id': game_id}, 
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
        'turn': turn,
        'result': result,
        'result_text': text,
        'to_move': to_move,
        'players': game['players'],
        'type': game['type']
      }
    }, 201
