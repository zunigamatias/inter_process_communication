const btnProcessOne = document.getElementById("btnProcessOne");
const btnProcessTwo = document.getElementById("btnProcessTwo");

const arrowP1P2 = document.getElementById("arrowP1P2")
const arrowP2P1 = document.getElementById("arrowP2P1")

btnProcessOne.addEventListener("click", () => {
    btnProcessOne.classList.add("btn-active")
    btnProcessTwo.classList.remove("btn-active")
    arrowP1P2.classList.add("arrow-active")
    arrowP2P1.classList.remove("arrow-active")
})
btnProcessTwo.addEventListener("click", () => {
    btnProcessOne.classList.remove("btn-active")
    btnProcessTwo.classList.add("btn-active")
    arrowP1P2.classList.remove("arrow-active")
    arrowP2P1.classList.add("arrow-active")
})


