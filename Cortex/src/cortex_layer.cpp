#include <flux/cortex/cortex_layer.h>
#include <algorithm>

const flux::CortexColumn &flux::CortexLayer::GrowOrMerge(const std::map<NeuralNodeId, NeuralNode> &context,
                                                         const std::map<MediatorId, MediatorValue> &mediators)
{
    for (auto &column : _columns)
    {
        if (column.TryMerge(context, mediators))
        {
            return column;
        }
    }

    _columns.emplace_back(_columns.size(), context, mediators);
    return _columns[_columns.size() - 1];
}

const std::vector<flux::CortexColumn> &flux::CortexLayer::GetExcitedColumns() const
{
    return _columns;
}

void flux::CortexLayer::Step(const std::map<NeuralNodeId, NeuralNode> &context,
                             const std::map<MediatorId, MediatorValue> &mediators)
{
    for (auto &column : _columns)
    {
        column.Step(context, mediators);
    }

    //Update excitement priorities: TODO: Only in neighbouring metric, neighbouring slices
    std::sort(_columns.begin(), _columns.end(), std::greater<CortexColumn>());
}
