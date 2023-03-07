


private void tsmiNew_Click(object sender, EventArgs e)
{
    // 判断当前文件是否从磁盘打开，或者新建时文档不为空，并且文件未被保存
    if (b == true || rtxtNotepad.Text.Trim() != "")
    {
        // 若文件未保存
        if (s == false)
        {
            string result;
            result = MessageBox.Show("文件尚未保存,是否保存?", 
                "保存文件", MessageBoxButtons.YesNoCancel).ToString();
            switch (result)
            {
                case "Yes":
                    // 若文件是从磁盘打开的
                    if (b == true)
                    {
                        // 按文件打开的路径保存文件
                        rtxtNotepad.SaveFile(odlgNotepad.FileName);
                    }
                    // 若文件不是从磁盘打开的
                    else if (sdlgNotepad.ShowDialog() == DialogResult.OK)
                    {
                        rtxtNotepad.SaveFile(sdlgNotepad.FileName);
                    }
                    s = true;
                    rtxtNotepad.Text = "";
                    break;
                case "No":
                    b = false;
                    rtxtNotepad.Text = "";
                    break;
            }
        }
    }
}

