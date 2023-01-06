#include <iostream>

#include "train.h"
#include "util.h"
#include "search.h"

void generateRandomGame(Board& board, bool printBoard)
{
    std::vector<int> availableMoves;

    for (int idx = 0; idx < BRD_SQ_NUM; idx++)
    {
        if (!outOfBounds(idx))
            availableMoves.push_back(idx);
    }

    shuffleVector(availableMoves);

    for (auto& idx : availableMoves)
    {
        board.makeMove(idx);

        if (board.state != BoardState::UNF)
        {
            if (printBoard)
                board.printBoard(true);

            break;
        }

        if (printBoard)
            board.printBoard(true);
    }
}

void trainNetwork(Network& net, int depth, int temp, int iteration, int batchSize,
    double (*activation)(double), double(*activationDerivative)(double), double lr, int printCycle)
{
    for (int i = 0; i < iteration; i++)
    {
        Board board;
        std::vector<int> results(3);
        double endPosError = 0;
        
        // batch.first.first = 2d vector of board, batch.first.second = length of the game(used to adjust lr)
        // batch.second.first = target(result of the game), batch.second.second = moveNum
        std::vector<std::pair<std::pair<std::vector<std::vector<int>>, int>, std::pair<std::vector<double>, int>>> batch;

        for (int j = 0; j < batchSize; j++)
        {
            batch.push_back(std::make_pair(std::make_pair(board.get2DVector(), 0), std::make_pair(std::vector<double>(3), 0)));

            while (board.state == BoardState::UNF)
            {
                SearchInfo info;
                std::pair<int, int> moveInfo = alphaBetaRoot(depth, board, net, info, board.getTurn(), temp, false);

                int moveIdx = moveInfo.first;

                board.makeMove(moveIdx);
                batch.push_back(std::make_pair(std::make_pair(board.get2DVector(), 0),
                                std::make_pair(std::vector<double>(3), board.getHist().size())));
            }

            std::vector<double> target(3);
            target[(int)board.state] = 1;
            endPosError += CrossEntropyError(target, Softmax(net.evaluate(board, activation)));

            results[(int)board.state]++;

            // set the result, length of the game
            // when there are 4 moves played, there are 4 + 1(empty board, starting position) positions to label
            for (int j = (int)(batch.size() - (board.getHist().size() + 1)); j < batch.size(); j++)
            {
                batch[j].first.second = (int)board.getHist().size(); // length
                batch[j].second.first[(int)board.state] = 1;    // result
            }

            // print the training game every <printCycle> games
            if (j % printCycle == 0)
            {
                std::cout << "game " << j << " of batch " << i << " completed." << std::endl;
                board.printBoard(true);

                std::vector<double> prob = Softmax(net.evaluate(board, activation));

                std::cout << "Eval of the final position: " <<
                    "Black win : " << prob[0] * 100 << "%, Draw : " << prob[1] * 100 << "%, White win : " << prob[2] * 100 << "%." << std::endl << std::endl;
            }

            board.clear();
        }

        double crossEntropyErrorSum = 0;

        for (int j = 0; j < batch.size(); j++)
        {
            std::vector<double> prob = Softmax(net.evaluate(batch[j].first.first, batch[j].second.second, activation));

            crossEntropyErrorSum += CrossEntropyError(batch[j].second.first, prob);
        }

        // shuffle the batch so that the net doesn't lean towards the result of the most recent games
        shuffleVector(batch);

        for (int j = 0; j < batch.size(); j++)
        {
            // divide the lr by the gameLen so all games get a equal contribution to the net, long or short
            net.backPropagate(batch[j].first.first, batch[j].second.second, batch[j].second.first,
                activation, activationDerivative, lr / batch[j].first.second);
        }

        net.update(batch.size());

        ASSERT(results[0] + results[1] + results[2] == batchSize);

        std::cout << std::endl;
        std::cout << batchSize << " games, " << results[0] << " Black wins, " << results[1] << " Draws, " << results[2] << " White wins." << std::endl;

        std::cout << "net training for batch " << i << " completed" << std::endl;

        std::vector<double> out = net.evaluate(board /*empty board*/, activation);
        std::vector<double> prob = Softmax(out);

        std::cout << "Empty board: Black win: " << prob[0] * 100 << "%, Draw: " << prob[1] * 100 << "%, White win: " << prob[2] * 100 << "%." << std::endl;
        std::cout << "Average Position Error: " << crossEntropyErrorSum / batch.size() << std::endl;
        std::cout << "Average End Position Error: " << endPosError / batchSize << std::endl;
    }
}