import QtQuick 2.0

ListModel {
    id: nodesModel
    ListElement {
        name: "event"
        icon: "/icons/solid/long-arrow-alt-down.svg"
        inputs: []
        outputs: [
            ListElement {
                name: "trigger"
                type: "impulse"
            }
        ]
        props: ListElement {}
    }
    ListElement {
        name: "timer"
        icon: "/icons/regular/clock.svg"
        inputs: []
        outputs: []
        props: ListElement {}
    }
    ListElement {
        name: "input test"
        icon: "/icons/solid/long-arrow-alt-down.svg"
        inputs: [
            ListElement {
                name: "trigger"
                type: "impulse"
            }
        ]
        outputs: []
        props: ListElement {}
    }
    ListElement {
        name: "input output test"
        icon: "/icons/solid/long-arrow-alt-down.svg"
        inputs: [
            ListElement {
                name: "trigger"
                type: "impulse"
            }
        ]
        outputs: [
            ListElement {
                name: "trigger"
                type: "impulse"
            }
        ]
        props: ListElement {}
    }
}
