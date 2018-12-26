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
using System.Windows.Media.Animation;

namespace TestAnimatedContainer
{
    /// <summary>
    /// Interaktionslogik für ElatecFloatingBox.xaml
    /// </summary>
    public partial class ElatecFloatingBox : UserControl
    {
        private uint place;

        public uint ModulCount
        {
            get
            {
                return place;
            }

            set
            {
                place = value;
            }
        }

        public ElatecFloatingBox(UInt32 place)
        {
            InitializeComponent();

            this.place = place;

            this.Loaded += ElatecFloatingBox_Loaded;
        }

        private void ElatecFloatingBox_Loaded(object sender, RoutedEventArgs e)
        {
            this.SortYourself();
        }

        public void SortYourself()
        {
            // Get Size Of Parent
            var parent = this.Parent;

            try
            {
                double pheight = ((Canvas)parent).ActualHeight;
                double pwidht = ((Canvas)parent).ActualWidth;

                if (place == 0)
                {
                    SetValue(Canvas.LeftProperty, pwidht / 2 - ActualWidth / 2 );
                    SetValue(Canvas.TopProperty, pheight / 2 - ActualHeight / 2);
                }
                else
                {
                    // In welcher Reihe befindet es sich?
                    int row = (int)Math.Floor(place / 4.0);
                    // In welcher Spalte befindet er sich
                    int col = (int)place % 3;
                    double xnew =   col * ActualWidth + 10;
                    double ynew = ((row) * ActualHeight) + 10;
                    transformYourself((int)xnew, (int)ynew, new TimeSpan(0, 0, 0, 0, 500));

                }


            }
            catch (Exception)
            {
            }
        }

        public void transformYourself(int newX, int newY, TimeSpan duration)
        {

            try
            {
                DoubleAnimation xAnimation = new DoubleAnimation();
                xAnimation.From = (double)GetValue(Canvas.LeftProperty);
                xAnimation.To = newX;
                xAnimation.Duration = new Duration(duration);
                DoubleAnimation yAnimation = new DoubleAnimation();
                yAnimation.From = (double)GetValue(Canvas.TopProperty);
                yAnimation.To = newY;
                yAnimation.Duration = new Duration(duration);
                this.BeginAnimation(Canvas.LeftProperty, xAnimation);
                this.BeginAnimation(Canvas.TopProperty, yAnimation);
                //TranslateTransform tx = new TranslateTransform();
                //tx.BeginAnimation(Canvas.TopProperty, xAnimation);
                //tx.BeginAnimation(Canvas.LeftProperty, yAnimation);
            }

            catch (Exception)
            {
            }

        }
    }

}
