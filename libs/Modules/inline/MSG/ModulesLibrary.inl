#ifdef MODULES_LIBRARY_HPP
namespace MSG {
inline void Dfs(
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

template <typename ModulesT>
inline auto GetAdj(const ModulesT& a_Modules)
{
    std::vector<std::vector<uint32_t>> adj(a_Modules.size());
    std::unordered_map<std::type_index, uint32_t> indiceLUT;
    for (uint32_t i = 0; i < a_Modules.size(); i++)
        indiceLUT[typeid(*a_Modules[i])] = i;
    for (uint32_t i = 0; i < a_Modules.size(); i++) {
        for (auto& dep : a_Modules[i]->dependencies)
            adj[i].emplace_back(indiceLUT.at(dep)); // this should crash if there is a missing subsystem
    }
    return adj;
}

template <typename ModulesT>
template <typename T, typename... Args>
inline void ModulesLibrary<ModulesT>::Add(Args&&... a_Args)
{
    auto systemPtr         = modules.emplace_back(std::make_shared<T>(std::forward<Args>(a_Args)...)).get();
    _modulesLUT[typeid(T)] = systemPtr;
}

template <typename ModulesT>
template <typename T>
inline void ModulesLibrary<ModulesT>::Remove()
{
    std::erase_if(
        modules,
        [](auto& a_Subsystem) { return typeid(*a_Subsystem) == typeid(T); });
    _modulesLUT.erase(typeid(T));
}

template <typename ModulesT>
template <typename T>
inline const T& ModulesLibrary<ModulesT>::Get() const
{
    return *reinterpret_cast<T*>(_modulesLUT.find(typeid(T))->second);
}

template <typename ModulesT>
inline void ModulesLibrary<ModulesT>::Sort()
{
    if (modules.empty())
        return;
    // do topological sort according to dependencies
    std::vector<bool> visited(modules.size(), false);
    std::vector<std::vector<uint32_t>> adj = GetAdj(modules);
    std::vector<uint32_t> ind;
    ind.reserve(modules.size());
    for (uint32_t i = 0; i < modules.size(); ++i) {
        if (!visited[i])
            Dfs(adj, ind, visited, i);
    }
    Modules res(modules.size());
    for (uint32_t i = 0; i < modules.size(); ++i)
        res[i] = std::move(modules[ind[i]]);
    modules = std::move(res);
}

template <typename ModulesT>
inline void ModulesLibrary<ModulesT>::Clear()
{
    modules.clear();
    _modulesLUT.clear();
}
}
#endif