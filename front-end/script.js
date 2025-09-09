document.addEventListener("DOMContentLoaded", () => {
  // Elements
  const btnProcessOne = document.getElementById("btnProcessOne");
  const btnProcessTwo = document.getElementById("btnProcessTwo");
  const arrowP1P2 = document.getElementById("arrowP1P2");
  const arrowP2P1 = document.getElementById("arrowP2P1");
  const selectEndpoint = document.getElementById("selectEndpoint");
  const textarea = document.getElementById("floatingTextarea2");
  const form = document.querySelector("form");
  const tableBody = document.querySelector("tbody");

  // sanity checks (use console if something is missing)
  if (!btnProcessOne || !btnProcessTwo || !textarea || !form || !tableBody) {
    console.error("Missing DOM elements:",
      { btnProcessOne, btnProcessTwo, textarea, form, tableBody });
    return;
  }

  // Toggle active process buttons
  btnProcessOne.addEventListener("click", () => {
    btnProcessOne.classList.add("btn-active");
    btnProcessTwo.classList.remove("btn-active");
    arrowP1P2.classList.add("arrow-active");
    arrowP2P1.classList.remove("arrow-active");
  });

  btnProcessTwo.addEventListener("click", () => {
    btnProcessOne.classList.remove("btn-active");
    btnProcessTwo.classList.add("btn-active");
    arrowP1P2.classList.remove("arrow-active");
    arrowP2P1.classList.add("arrow-active");
  });

  // optional: Bootstrap validation handler (only if you have .needs-validation forms)
  (function () {
    var forms = document.querySelectorAll('.needs-validation');
    Array.prototype.slice.call(forms).forEach(function (f) {
      f.addEventListener('submit', function (event) {
        if (!f.checkValidity()) {
          event.preventDefault();
          event.stopPropagation();
        }
        f.classList.add('was-validated');
      }, false);
    });
  })();

  // Submit handler: send to bridge and update table with response
  form.addEventListener("submit", async (event) => {
    event.preventDefault();

    const endpoint = selectEndpoint ? selectEndpoint.value : "anonymousPipes";
    const messageText = textarea.value.trim();
    if (!messageText) return; // nothing to send

    const mainProcess = btnProcessOne.classList.contains("btn-active") ? "A" : "B";

    const req = {
      id: Date.now(),
      endCommunication: false,
      message: {
        endpoint: endpoint,
        mainProcess: mainProcess,
        message: messageText
      }
    };

    try {
      const response = await fetch("http://localhost:3000/send", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(req)
      });

      if (!response.ok) throw new Error("HTTP error " + response.status);

      const data = await response.json();
      console.log("Backend response:", data);

      // append row to table
      const row = document.createElement("tr");
      const indexCell = document.createElement("th");
      indexCell.scope = "row";
      indexCell.textContent = tableBody.rows.length + 1;

      const senderCell = document.createElement("td");
      senderCell.dataset.label = "Sender:";
      senderCell.textContent = mainProcess;

      const receiverCell = document.createElement("td");
      receiverCell.dataset.label = "Receiver:";
      receiverCell.textContent = "Backend";

      const messageCell = document.createElement("td");
      messageCell.dataset.label = "Message:";
      messageCell.textContent = data.msg ?? "(no msg)";

      row.appendChild(indexCell);
      row.appendChild(senderCell);
      row.appendChild(receiverCell);
      row.appendChild(messageCell);
      tableBody.appendChild(row);

      // clear textarea
      textarea.value = "";

    } catch (err) {
      console.error("Error sending request:", err);
      alert("Error sending request: " + err.message);
    }
  });
});
