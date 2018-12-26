using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.Collections.Concurrent;
using System.ComponentModel;

namespace TestAnimatedContainer
{

    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ObservableCollection<ElatecFloatingBox> modullist = new ObservableCollection<ElatecFloatingBox>();

    #region Constructor
        public MainWindow()
        {
            InitializeComponent();
        }
    #endregion

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            ElatecFloatingBox newmodul = new ElatecFloatingBox(0);
            AddModul(newmodul);
        }

        #region Module Helper Functions
        private void AddModul(ElatecFloatingBox newmodule)
        {
            for (int i = 0; i < modullist.Count; i++)
            {
                modullist[i].ModulCount = ++modullist[i].ModulCount;
                modullist[i].SortYourself();
            }
            modullist.Add(newmodule);
            canvasFloatingBox.Children.Add(newmodule);
        }
        private void RemoveModul(int pos)
        {

            modullist.RemoveAt(pos);
            canvasFloatingBox.Children.RemoveAt(pos);
        }
        #endregion
    }


    public class ModulContainer : INotifyPropertyChanged
    {
        public ElatecFloatingBox _Modul;

        public ElatecFloatingBox Modul
        {
            get
            {
                return Modul;
            }

            set
            {
                APropertyChanged("Modul");
                _Modul = value;
            }
        }

        public void APropertyChanged(string message)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(message));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
