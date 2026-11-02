// Realce de sintaxe dos blocos de código que o Rouge (GitHub Pages) não
// conhece: ```imagemlang, ```saida (saída do compilador) e ```ebnf.
(function () {
  "use strict";

  var PALAVRAS_CHAVE = [
    "imagem", "inteiro", "redimensionar", "para", "por", "tons_de_cinza",
    "rotacionar", "recortar", "de", "tamanho", "salvar", "como"
  ];

  function escapar(texto) {
    return texto.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
  }

  function span(classe, texto) {
    return '<span class="' + classe + '">' + escapar(texto) + "</span>";
  }

  // Aplica 'regra' (expressão com grupos) ao texto; cada grupo casado vira um
  // <span> com a classe correspondente em 'classes'.
  function tokenizar(texto, regra, classes, classificar) {
    var saida = "";
    var ultimo = 0;
    var m;

    regra.lastIndex = 0;
    while ((m = regra.exec(texto)) !== null) {
      if (m[0] === "") {
        regra.lastIndex++;
        continue;
      }
      saida += escapar(texto.slice(ultimo, m.index));
      for (var i = 1; i < m.length; i++) {
        if (m[i] !== undefined) {
          var classe = classificar ? classificar(i, m[i]) : classes[i - 1];
          saida += classe ? span(classe, m[i]) : escapar(m[i]);
          break;
        }
      }
      ultimo = regra.lastIndex;
    }
    return saida + escapar(texto.slice(ultimo));
  }

  function imagemlang(texto) {
    var regra = /(\/\/[^\n]*)|("(?:[^"\\\n]|\\.)*"?)|(\b\d+\b)|(\b[A-Za-z_]\w*\b)|([=;,])/g;
    return tokenizar(texto, regra, null, function (grupo, valor) {
      if (grupo === 4)
        return PALAVRAS_CHAVE.indexOf(valor) >= 0 ? "il-kw" : "il-id";
      return ["il-com", "il-str", "il-num", null, "il-op"][grupo - 1];
    });
  }

  function saida(texto) {
    return texto.split("\n").map(function (linha) {
      var erro = /^(\S+:\d+:\d+:)( erro (?:lexico|sintatico|semantico):)(.*)$/.exec(linha);
      if (erro)
        return span("il-pos", erro[1]) + span("il-err", erro[2]) + escapar(erro[3]);
      if (/^analise (lexica )?concluida sem erros\.$/.test(linha) || /^PASSOU\b/.test(linha))
        return span("il-ok", linha);
      if (/^analise (lexica )?concluida com /.test(linha) || /^FALHOU\b/.test(linha))
        return span("il-warn", linha);
      return escapar(linha);
    }).join("\n");
  }

  function ebnf(texto) {
    var regra = /(\(\*[\s\S]*?\*\))|("[^"\n]*"|'[^'\n]*')|(\b[A-Za-z_]\w*\b)|([=|;,{}\[\]()])/g;
    return tokenizar(texto, regra, ["il-com", "il-str", "il-nt", "il-op"]);
  }

  var REALCES = { imagemlang: imagemlang, saida: saida, ebnf: ebnf };

  function realcar() {
    Object.keys(REALCES).forEach(function (lingua) {
      var blocos = document.querySelectorAll(
        ".language-" + lingua + " code, code.language-" + lingua);
      Array.prototype.forEach.call(blocos, function (bloco) {
        if (bloco.dataset.realcado)
          return;
        bloco.innerHTML = REALCES[lingua](bloco.textContent);
        bloco.dataset.realcado = "1";
      });
    });
  }

  if (document.readyState === "loading")
    document.addEventListener("DOMContentLoaded", realcar);
  else
    realcar();
})();
