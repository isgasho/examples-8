module Query = [%relay.query
  {|
    query pagesQuery {
      books {
        title
        author
        comments @defer {
          user
          text
        }
      }
    }
  |}
];

module Comments = {
  [@react.component]
  let make = (~comments) => {
    Query.Types.(
      comments
      ->Belt.Option.getWithDefault([||])
      ->Belt.Array.map(comment => {
          let text = comment.text;
          let user = comment.user;
          <li key=text> {j|$user: $text|j}->React.string </li>;
        })
      ->React.array
    );
  };
};

module Book = {
  [@react.component]
  let make = (~title, ~author, ~comments) => {
    <div>
      <p> {j|$title by $author|j}->React.string </p>
      <Comments comments />
    </div>;
  };
};

module Books = {
  [@react.component]
  let make = () => {
    let response = Query.use(~variables=(), ());
    let booksCount = response.books->Js.Array.length;
    Js.log2("books: ", response.books);

    <div>
      <h2
        className="text-4xl font-extrabold tracking-tight text-gray-900 leading-10 sm:text-5xl sm:leading-none md:text-6xl pb-10">
        {j|Streaming $booksCount books....|j}->React.string
      </h2>
      {response.books
       ->Belt.Array.map(book =>
           <Book
             key={book.title}
             title={book.title}
             author={book.author}
             comments={book.comments}
           />
         )
       ->React.array}
    </div>;
  };
};

[@react.component]
let make = () => {
  let environment = RelayEnv.initEnvironment(~queryRecords=None);

  <div
    className="flex flex-col items-center justify-center h-full px-4 mx-auto text-center bg-white">
    <ReasonRelay.Context.Provider environment>
      <ErrorBoundary
        fallbackComponent={<div> "not found"->React.string </div>}>
        <ReactExperimental.Suspense
          fallback={<div> {React.string("Loading...")} </div>}>
          <Books />
        </ReactExperimental.Suspense>
      </ErrorBoundary>
    </ReasonRelay.Context.Provider>
  </div>;
};

let default = make;
