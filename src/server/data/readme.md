## m_plan
&emsp;&emsp;`Mark` 数据结构的最小组成部分：计划。</br>
&emsp;&emsp;每一个 `m_plan` 实例表示一条计划，其中储存了此计划的信息。

## m_user
&emsp;&emsp;`Mark` 数据结构的用户部分。</br>
&emsp;&emsp;每一个 `m_user` 实例表示一个用户，其中额外储存了属于此用户的计划id，可通过计划id在 `m_group` 的map中获取计划的内容。

## m_group
&emsp;&emsp;`Mark` 数据结构的最外层部分：组。</br>
&emsp;&emsp;每一个 `m_group` 实例代表一个计划组。一个计划组中储存了若干属于此组的用户，而这些用户的计划亦均储存于此组。</br>
&emsp;&emsp;组内计划由一张map维护，键为计划id，值为 `m_plan`。因为 `m_user` 中额外储存了所属于此用户的计划id，所以通过遍历其中的计划id，即可获取所属于此用户的计划内容。

**小结:**
1. `m_group` 中储存了若干 `m_user` 和 `m_plan`，两者分别对应所属于此组的用户和计划。
2. `m_user` 中储存了所属于此用户的计划id，但不储存计划实体，需通过所属 `m_group` 的map来获取计划实体。
3. `m_plan` 为最底层单位，一个实例即一个计划实体。
