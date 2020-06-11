import QtQuick 2.14
import QtQuick.Controls 2.14

import '.' as Sky

Sky.Text {
    property int textIndent: 0
    property int paragraphSpacing: 0

    onHeightChanged: recomputeLorem()
    onWidthChanged: recomputeLorem()
    onTextIndentChanged: recomputeLorem()
    onParagraphSpacingChanged: recomputeLorem()
    onLineHeightChanged: recomputeLorem()
    onFontChanged: recomputeLorem()

    function p(text) {
        return `<p style="margin-bottom: ` + paragraphSpacing + `px; text-indent: ` + textIndent + `px;">` + text + `</p>`
    }

    function recomputeLorem() {
        if (!checkFits()) {
            return;
        }

        text = p("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer enim eros, dignissim at varius feugiat, eleifend id metus. Quisque a tincidunt arcu. Mauris vel ligula nibh. Nulla facilisi.");

        if (!checkFits()) {
            return;
        }

        text += p("Maecenas sodales viverra sem nec consequat. Maecenas facilisis, diam eget malesuada vulputate, dolor tellus interdum est, eu rhoncus massa nunc eu est. Vivamus bibendum sollicitudin ligula, sit amet convallis arcu luctus sit amet. Phasellus consectetur urna urna, quis rhoncus ante sollicitudin eu. Fusce elementum tellus ut tristique posuere. Fusce maximus eget libero in aliquam. Fusce nec velit non nulla volutpat tincidunt. Phasellus odio purus, porttitor et ipsum et, egestas facilisis mauris. Vivamus faucibus sem at leo rhoncus, a varius mi aliquam. Sed vel lacus ante. Praesent imperdiet, tellus id venenatis cursus, libero ligula posuere tortor, eu viverra nisi massa vitae metus. Duis id varius nisi, faucibus laoreet erat.");

        if (!checkFits()) {
            return;
        }

        text += p("Phasellus vitae felis condimentum erat dignissim congue. Nulla eget est et lorem tincidunt venenatis nec a ipsum. Nullam imperdiet tincidunt urna pellentesque aliquam. Etiam lorem sem, rutrum eget dapibus at, hendrerit eget diam. In a feugiat turpis. Suspendisse vitae ultrices est. Aliquam auctor, purus vel egestas eleifend, felis orci rutrum felis, et fringilla mi purus id diam. Fusce pretium varius vestibulum. Nulla in congue lectus. Etiam at malesuada quam.");

        if (!checkFits()) {
            return;
        }

        text += p("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris blandit nulla quam, eu tincidunt ipsum ultrices ac. Nam quis dolor non urna finibus bibendum. Nulla id tortor ornare, iaculis nisl quis, convallis lacus. Nam sodales odio ac pharetra lobortis. Mauris leo erat, faucibus nec gravida eu, commodo ac dui. Cras ultricies posuere velit, eu placerat velit. Sed in tempor mauris. Suspendisse potenti. Aliquam luctus erat varius ultricies tristique. Suspendisse tempus efficitur odio, et placerat metus.");

        if (!checkFits()) {
            return;
        }

        text += p("Vivamus eleifend fringilla urna, a sollicitudin ex sodales nec. Suspendisse nec erat purus. Quisque ullamcorper ex vitae risus consequat posuere. Nulla eu ultricies lorem, nec lacinia justo. In sollicitudin volutpat ipsum, a varius nisi volutpat nec. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Nullam sed malesuada velit, ac porta nulla. Fusce nec erat nec turpis efficitur finibus. Nunc at tempor sem, at luctus tortor. Phasellus molestie in nisl sit amet finibus. Praesent in faucibus mi. In ultricies molestie libero, sit amet vulputate orci dapibus eget. Proin accumsan arcu elit, ultricies interdum risus interdum et. Quisque commodo, velit vitae pharetra iaculis, libero turpis congue urna, eget lobortis diam nibh nec quam. Nulla sed congue neque.");

        if (!checkFits()) {
            return;
        }

        text += p("Vestibulum lacus tortor, vehicula ac elit vel, hendrerit dictum dolor. Nullam et justo sagittis, tempor nibh id, ullamcorper ex. Etiam a malesuada est. Nullam odio nisl, iaculis non vestibulum eu, tincidunt ac diam. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Duis dictum metus sed ipsum mollis, tristique feugiat neque tempor. Praesent et egestas metus. Phasellus quis sem ante. Duis mollis orci at maximus volutpat. Etiam fermentum suscipit congue. Morbi pellentesque metus quis sapien sagittis, sit amet vulputate diam dictum.");

        if (!checkFits()) {
            return;
        }

        text += p("Morbi rutrum velit tellus, nec posuere augue dictum et. Praesent sollicitudin sagittis augue eu finibus. Maecenas nec mauris lorem. Duis risus leo, viverra et facilisis non, laoreet dapibus velit. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Fusce sit amet congue erat. Nunc feugiat eleifend libero, vitae lacinia eros luctus sed. Donec a ullamcorper ipsum. Nulla sollicitudin semper leo ut tempus. Nullam rhoncus volutpat ex eget tempor. Aliquam ac vulputate dui. Suspendisse potenti. Nullam condimentum nulla vel velit feugiat congue non sit amet nisi. Duis cursus dui at leo imperdiet bibendum.");
    }

    function checkFits() {
        return height > 0 && width > 0 && contentHeight <= height && contentWidth <= width;
    }
}
