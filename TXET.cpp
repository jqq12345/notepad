


private void tsmiNew_Click(object sender, EventArgs e)
{
    // �жϵ�ǰ�ļ��Ƿ�Ӵ��̴򿪣������½�ʱ�ĵ���Ϊ�գ������ļ�δ������
    if (b == true || rtxtNotepad.Text.Trim() != "")
    {
        // ���ļ�δ����
        if (s == false)
        {
            string result;
            result = MessageBox.Show("�ļ���δ����,�Ƿ񱣴�?", 
                "�����ļ�", MessageBoxButtons.YesNoCancel).ToString();
            switch (result)
            {
                case "Yes":
                    // ���ļ��ǴӴ��̴򿪵�
                    if (b == true)
                    {
                        // ���ļ��򿪵�·�������ļ�
                        rtxtNotepad.SaveFile(odlgNotepad.FileName);
                    }
                    // ���ļ����ǴӴ��̴򿪵�
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

