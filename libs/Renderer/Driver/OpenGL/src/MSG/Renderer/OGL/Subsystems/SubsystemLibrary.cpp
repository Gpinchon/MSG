#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemLibrary.hpp>

void Dfs(
    const std::vector<std::vector<uint32_t>>& a_Adj,
    std::vector<uint32_t>& a_Ind,
    std::vector<bool>& a_Visited,
    uint32_t v)
{
    a_Visited[v] = true;
    for (uint32_t u : a_Adj[v]) {
        if (!a_Visited[u])
            Dfs(a_Adj, a_Ind, a_Visited, u);
    }
    a_Ind.emplace_back(v);
}

auto GetAdj(const MSG::Renderer::Subsystems& a_Subsystems)
{
    std::vector<std::vector<uint32_t>> adj(a_Subsystems.size());
    std::unordered_map<std::type_index, uint32_t> indiceLUT;
    for (uint32_t i = 0; i < a_Subsystems.size(); i++)
        indiceLUT[typeid(*a_Subsystems[i])] = i;
    for (uint32_t i = 0; i < a_Subsystems.size(); i++) {
        for (auto& dep : a_Subsystems[i]->dependencies)
            adj[i].emplace_back(indiceLUT.at(dep)); // this should crash if there is a missing subsystem
    }
    return adj;
}

void MSG::Renderer::SubsystemLibrary::Sort()
{
    if (subsystems.empty())
        return;
    // do topological sort according to dependencies
    std::vector<bool> visited(subsystems.size(), false);
    std::vector<std::vector<uint32_t>> adj = GetAdj(subsystems);
    std::vector<uint32_t> ind;
    ind.reserve(subsystems.size());
    for (uint32_t i = 0; i < subsystems.size(); ++i) {
        if (!visited[i])
            Dfs(adj, ind, visited, i);
    }
    MSG::Renderer::Subsystems res(subsystems.size());
    for (uint32_t i = 0; i < subsystems.size(); ++i)
        res[i] = std::move(subsystems[ind[i]]);
    subsystems = std::move(res);
}