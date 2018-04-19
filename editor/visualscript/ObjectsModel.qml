import QtQuick 2.0

ListModel {
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
        name: "component"
        icon: "/icons/solid/table.svg"
        inputs: []
        outputs: [
            ListElement {
                name: "data"
                type: "data"
            }
        ]
        props: ListElement {}
    }
    ListElement {
        name: "commit"
        icon: "/icons/solid/download.svg"
        inputs: [
            ListElement {
                name: "data"
                type: "data"
            }
        ]
        outputs: []
        props: ListElement {}
    }
    ListElement {
        name: "entity"
        icon: "/icons/solid/id-badge.svg"
        inputs: []
        outputs: [
            ListElement {
                name: "entity"
                type: "eid"
            }
        ]
        props: ListElement {}
    }
    ListElement {
        name: "signal"
        icon: "/icons/solid/rss.svg"
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
