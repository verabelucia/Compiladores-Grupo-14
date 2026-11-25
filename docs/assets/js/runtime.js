(function () {
  "use strict";

  function criar(tag, classe, texto) {
    var el = document.createElement(tag);
    if (classe) el.className = classe;
    if (texto !== undefined) el.textContent = texto;
    return el;
  }

  function demo(nome) {
    return document.querySelector('[data-demo="' + nome + '"]');
  }

  function linhas(el, textos) {
    el.textContent = "";
    textos.forEach(function (t) {
      el.appendChild(criar("div", t[1] || "", t[0]));
    });
  }

  function memoria() {
    var raiz = demo("memoria");
    if (!raiz) return;
    var largura = 4, altura = 3, canais = 3;
    var grade = raiz.querySelector("[data-grade]");
    var fita = raiz.querySelector("[data-fita]");
    var info = raiz.querySelector("[data-info]");
    var cores = [];
    var botoes = [];
    var bytes = [];

    for (var i = 0; i < largura * altura; i++)
      cores.push([60 + (i % 4) * 60, 70 + Math.floor(i / 4) * 70, 230 - i * 14]);

    function escolher(i) {
      var x = i % largura, y = Math.floor(i / largura);
      var inicio = i * canais;
      botoes.forEach(function (b, k) {
        b.classList.toggle("rt-ativo", k === i);
        b.setAttribute("aria-pressed", k === i ? "true" : "false");
      });
      bytes.forEach(function (b, k) {
        b.classList.toggle("rt-ativo", k >= inicio && k < inicio + canais);
      });
      linhas(info, [
        ["pixel (" + x + ", " + y + ")", "rt-forte"],
        ["índice = (y * largura + x) * canais"],
        ["índice = (" + y + " * " + largura + " + " + x + ") * " + canais + " = " + inicio],
        ["bytes " + inicio + ", " + (inicio + 1) + " e " + (inicio + 2) + ": R=" +
          cores[i][0] + " G=" + cores[i][1] + " B=" + cores[i][2]]
      ]);
    }

    grade.style.gridTemplateColumns = "repeat(" + largura + ", 3rem)";
    cores.forEach(function (c, i) {
      var b = criar("button", "rt-celula", (i % largura) + "," + Math.floor(i / largura));
      b.type = "button";
      b.style.background = "rgb(" + c.join(",") + ")";
      b.addEventListener("click", function () { escolher(i); });
      b.addEventListener("mouseenter", function () { escolher(i); });
      b.addEventListener("focus", function () { escolher(i); });
      botoes.push(b);
      grade.appendChild(b);
      c.forEach(function (v, k) {
        var byte = criar("span", "rt-byte rt-" + "rgb"[k], String(v));
        bytes.push(byte);
        fita.appendChild(byte);
      });
    });
    escolher(6);
  }

  function cinza() {
    var raiz = demo("cinza");
    if (!raiz) return;
    var entradas = raiz.querySelectorAll("input");
    var info = raiz.querySelector("[data-info]");

    function atualizar() {
      var v = {};
      Array.prototype.forEach.call(entradas, function (e) {
        v[e.dataset.c] = Number(e.value);
        e.parentNode.querySelector("output").textContent = e.value;
      });
      var y = Math.floor((299 * v.r + 587 * v.g + 114 * v.b + 500) / 1000);
      var m = Math.round((v.r + v.g + v.b) / 3);
      raiz.querySelector("[data-cor]").style.background = "rgb(" + v.r + "," + v.g + "," + v.b + ")";
      raiz.querySelector("[data-cinza]").style.background = "rgb(" + y + "," + y + "," + y + ")";
      raiz.querySelector("[data-media]").style.background = "rgb(" + m + "," + m + "," + m + ")";
      linhas(info, [
        ["(299 * " + v.r + " + 587 * " + v.g + " + 114 * " + v.b + " + 500) / 1000 = " + y, "rt-forte"],
        ["média simples: " + m]
      ]);
    }

    Array.prototype.forEach.call(entradas, function (e) {
      e.addEventListener("input", atualizar);
    });
    atualizar();
  }

  function rotacao() {
    var raiz = demo("rotacao");
    if (!raiz) return;
    var grade = raiz.querySelector("[data-grade]");
    var info = raiz.querySelector("[data-info]");
    var botoes = raiz.querySelectorAll("button");
    var img, timer, historico;

    function reiniciar() {
      clearTimeout(timer);
      img = { l: 3, a: 2, d: ["A", "B", "C", "D", "E", "F"] };
      historico = [["início: 3x2, A em (0, 0)"]];
    }

    function girar() {
      var l = img.a, a = img.l, d = new Array(l * a);
      for (var y = 0; y < img.a; y++)
        for (var x = 0; x < img.l; x++)
          d[x * l + (img.a - 1 - y)] = img.d[y * img.l + x];
      var antes = img.d.indexOf("A");
      var ax = antes % img.l, ay = Math.floor(antes / img.l);
      historico.push(["giro " + historico.length + ": " + l + "x" + a + ", A: (" + ax + ", " + ay +
        ") → (" + img.a + " - 1 - " + ay + ", " + ax + ") = (" + (img.a - 1 - ay) + ", " + ax + ")"]);
      img = { l: l, a: a, d: d };
    }

    function desenhar() {
      grade.textContent = "";
      grade.style.gridTemplateColumns = "repeat(" + img.l + ", 3rem)";
      img.d.forEach(function (letra) {
        var c = criar("div", "rt-celula rt-letra-" + letra.toLowerCase(), letra);
        if (letra === "A") c.classList.add("rt-ativo");
        grade.appendChild(c);
      });
      linhas(info, historico);
    }

    Array.prototype.forEach.call(botoes, function (b) {
      b.addEventListener("click", function () {
        var faltam = Number(b.dataset.ang) / 90;
        reiniciar();
        desenhar();
        (function passo() {
          if (faltam-- <= 0) return;
          timer = setTimeout(function () {
            girar();
            desenhar();
            passo();
          }, 500);
        })();
      });
    });
    reiniciar();
    desenhar();
  }

  function recorte() {
    var raiz = demo("recorte");
    if (!raiz) return;
    var largura = 12, altura = 8;
    var grade = raiz.querySelector("[data-grade]");
    var info = raiz.querySelector("[data-info]");
    var entradas = raiz.querySelectorAll("input");
    var celulas = [];

    grade.style.gridTemplateColumns = "repeat(" + largura + ", 1.15rem)";
    for (var i = 0; i < largura * altura; i++) {
      celulas.push(criar("div", "rt-celula"));
      grade.appendChild(celulas[i]);
    }

    function atualizar() {
      var v = {};
      Array.prototype.forEach.call(entradas, function (e) {
        v[e.dataset.c] = Number(e.value);
        e.parentNode.querySelector("output").textContent = e.value;
      });
      var okX = v.x + v.l <= largura, okY = v.y + v.a <= altura;
      celulas.forEach(function (c, i) {
        var x = i % largura, y = Math.floor(i / largura);
        var dentro = x >= v.x && x < v.x + v.l && y >= v.y && y < v.y + v.a;
        c.classList.toggle("rt-dentro", dentro && okX && okY);
        c.classList.toggle("rt-fora", dentro && !(okX && okY));
      });
      linhas(info, [
        [(okX ? "✓ " : "✗ ") + v.x + " + " + v.l + " = " + (v.x + v.l) + (okX ? " ≤ " : " > ") + largura, okX ? "rt-sim" : "rt-nao"],
        [(okY ? "✓ " : "✗ ") + v.y + " + " + v.a + " = " + (v.y + v.a) + (okY ? " ≤ " : " > ") + altura, okY ? "rt-sim" : "rt-nao"],
        [okX && okY
          ? "recorte aceito: a imagem passa a ter " + v.l + "x" + v.a
          : "erro de execucao: recortar: area de " + v.x + ", " + v.y + " tamanho " + v.l + "x" + v.a +
            " fora da imagem " + largura + "x" + altura, "rt-forte"]
      ]);
    }

    Array.prototype.forEach.call(entradas, function (e) {
      e.addEventListener("input", atualizar);
    });
    atualizar();
  }

  function iniciar() {
    memoria();
    cinza();
    rotacao();
    recorte();
  }

  if (document.readyState === "loading")
    document.addEventListener("DOMContentLoaded", iniciar);
  else
    iniciar();
})();
