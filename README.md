# QuickQuiz
答题软件

## 更新日志
#### 2021/8/20提交最初版本V2.0
1. 取消了智能识别功能中，选项号后有空格也识别为新的选项（避免在识别全英文题目时错误识别）
2. 修正了批量生成中答案可能探测不到的问题（题干中有多个括号时）
3. 删除了不适合公开的内容

#### 2021/8/22提交修改V2.0
感谢52网友swagger的测试
修正以下问题：
1. 新建题目时无法批量删除的问题
2. 去掉了编辑题目时不允许新增和删除题目的限制
3. 修正了新建选择题不输入选项而新建时导致假死的问题
4. 新建题目时切换单选题和多选题自动拷贝选项内容

#### 2021/8/26提交修改V2.0
感谢52网友sky9131986的测试
修正以下问题：
1. 修正了对类似“12.345”题目去题号时，会将点号后面的数字一起去掉的问题
2. 修正了新建题库后收藏题目，不保存题库仍会生成收藏夹文件的问题
3. 复制题目将复制收藏信息

#### 2021/9/13提交修改V2.0
感谢网友ahit976的测试
修正以下问题：
1. 添加自定义类型题目会导致闪退的问题。
